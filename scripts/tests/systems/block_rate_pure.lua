-----------------------------------
-- Pure system tests for shield block dual-wire helpers (slice 6688).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/blockrate (0839 / 6087 / 6211).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical

describe('block rate constants and shield table', function()
    it('pins sizes and coeffs', function()
        assert(phys.blockRateMin == 5)
        assert(phys.blockRateMax == 100)
        assert(math.abs(phys.blockSkillDeltaCoeff - 0.2325) < 1e-12)
        assert(math.abs(phys.automatonBlockSkillDeltaCoeff - 0.215) < 1e-12)
        assert(phys.reprisalSkillScale == 1.15)
        assert(phys.reprisalMultDefault == 1.5)
        assert(phys.reprisalMultBonus == 3.0)
        assert(phys.blockRateFromShieldSize(0) == 0)
        assert(phys.blockRateFromShieldSize(1) == 55)
        assert(phys.blockRateFromShieldSize(2) == 40)
        assert(phys.blockRateFromShieldSize(3) == 45)
        assert(phys.blockRateFromShieldSize(4) == 30)
        assert(phys.blockRateFromShieldSize(5) == 50)
        assert(phys.blockRateFromShieldSize(6) == 100)
        assert(phys.blockRateFromShieldSize(7) == 0)
    end)
end)

describe('canBlockFromParams', function()
    it('requires facing and no prevent-action', function()
        assert(not phys.canBlockFromParams({
            facing = false, isPC = true, shieldSkillRank = 1, hasSubItem = true, subIsShield = true,
        }))
        assert(not phys.canBlockFromParams({
            facing = true, preventAction = true, isPC = true, shieldSkillRank = 1, hasSubItem = true, subIsShield = true,
        }))
    end)

    it('PC needs shield skill and sub shield', function()
        assert(phys.canBlockFromParams({
            facing = true, isPC = true, shieldSkillRank = 1, hasSubItem = true, subIsShield = true,
        }))
        assert(not phys.canBlockFromParams({
            facing = true, isPC = true, shieldSkillRank = 0, hasSubItem = true, subIsShield = true,
        }))
        assert(not phys.canBlockFromParams({
            facing = true, isPC = true, shieldSkillRank = 1, hasSubItem = false, subIsShield = false,
        }))
    end)

    it('mob/pet/trust needs CAN_SHIELD_BLOCK', function()
        assert(phys.canBlockFromParams({
            facing = true, isMobPetOrTrust = true, canShieldBlockMod = 1,
        }))
        assert(not phys.canBlockFromParams({
            facing = true, isMobPetOrTrust = true, canShieldBlockMod = 0,
        }))
        assert(not phys.canBlockFromParams({ facing = true, isPC = false }))
    end)
end)

describe('blockRateFromParams PC', function()
    it('returns 0 without shield', function()
        assert(phys.blockRateFromParams({
            kind = 'pc', hasShield = false, shieldSize = 1, blockSkill = 300, attackSkill = 250,
        }) == 0)
    end)

    it('buckler equal skill is 55', function()
        assert(phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 1, blockSkill = 300, attackSkill = 300,
        }) == 55)
    end)

    it('applies skill delta, floor, ceiling, palisade, reprisal', function()
        -- kite 45 + 100*0.2325 = 68.25
        local got = phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 3, blockSkill = 350, attackSkill = 250,
        })
        assert(math.abs(got - (45 + 100 * 0.2325)) < 1e-9)

        -- tower low skill → floor 5
        assert(phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 4, blockSkill = 0, attackSkill = 500,
        }) == 5)

        -- ochain high → cap 100
        assert(phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 6, blockSkill = 500, attackSkill = 0, palisadeMod = 50,
        }) == 100)

        -- round + palisade 20, equal skill → 60
        assert(phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 2, blockSkill = 100, attackSkill = 100, palisadeMod = 20,
        }) == 60)

        -- reprisal 1.5× on buckler equal 200
        local skill = 200 * 1.15
        local want = (55 + (skill - 200) * 0.2325) * 1.5
        got = phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 1, blockSkill = 200, attackSkill = 200, hasReprisal = true,
        })
        assert(math.abs(got - want) < 1e-9)

        -- Adamas/Priwen 3× clamps to 100
        got = phys.blockRateFromParams({
            kind = 'pc', hasShield = true, shieldSize = 1, blockSkill = 200, attackSkill = 200,
            hasReprisal = true, reprisalBlockBonus = true,
        })
        want = (55 + (skill - 200) * 0.2325) * 3.0
        if want > 100 then
            want = 100
        end
        assert(math.abs(got - want) < 1e-9)
    end)
end)

describe('blockRateFromParams mob and automaton', function()
    it('mob without can-block is 0; with base+skill delta', function()
        assert(phys.blockRateFromParams({
            kind = 'mob', canShieldBlock = false, baseBlockRate = 40, blockSkill = 300, attackSkill = 200,
        }) == 0)
        local got = phys.blockRateFromParams({
            kind = 'mob', canShieldBlock = true, baseBlockRate = 40, blockSkill = 300, attackSkill = 200,
        })
        assert(math.abs(got - (40 + 100 * 0.2325)) < 1e-9)
    end)

    it('automaton early path skips clamp and uses 0.215', function()
        -- max(0, 30 + (250-200)*0.215) = 40.75
        local got = phys.blockRateFromParams({
            kind = 'automaton', canShieldBlock = true, baseBlockRate = 30,
            automatonMeleeSkill = 250, attackSkill = 200,
        })
        assert(math.abs(got - 40.75) < 1e-9)
        -- no can-block
        assert(phys.blockRateFromParams({
            kind = 'automaton', canShieldBlock = false, baseBlockRate = 30, automatonMeleeSkill = 250, attackSkill = 200,
        }) == 0)
        -- skill deficit floors at 0
        assert(phys.blockRateFromParams({
            kind = 'automaton', canShieldBlock = true, baseBlockRate = 5,
            automatonMeleeSkill = 0, attackSkill = 1000,
        }) == 0)
    end)
end)

describe('damageReductionForBlockFromParams', function()
    it('returns flat reduction', function()
        assert(phys.damageReductionForBlockFromParams({ damage = 0 }) == 0)
        assert(phys.damageReductionForBlockFromParams({ damage = -5 }) == 0)
        -- non-PC: floor(100*0.5)=50 reduction 50
        assert(phys.damageReductionForBlockFromParams({ damage = 100, isPC = false }) == 50)
        -- PC absorb rate 40 → keep 60%: floor(100*0.6)=60 → reduction 40
        assert(phys.damageReductionForBlockFromParams({
            damage = 100, isPC = true, shieldAbsorbRate = 40,
        }) == 40)
        -- shield def bonus first: 100-20=80 → floor(80*0.5)=40 → reduction 60
        assert(phys.damageReductionForBlockFromParams({
            damage = 100, isPC = false, shieldDefBonus = 20,
        }) == 60)
    end)
end)

describe('blockSucceeds and attacker skill type', function()
    it('compares rate*100 to d10000 roll', function()
        assert(phys.blockSucceeds(100, 10000))
        assert(phys.blockSucceeds(50, 5000))
        assert(not phys.blockSucceeds(50, 5001))
        assert(not phys.blockSucceeds(0, 1))
    end)

    it('defaults H2H skill type when using H2H', function()
        assert(phys.attackerSkillTypeForBlock(true, 3) == xi.skill.HAND_TO_HAND)
        assert(phys.attackerSkillTypeForBlock(false, 3) == 3)
    end)
end)
