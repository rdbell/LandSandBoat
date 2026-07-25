-----------------------------------
-- Pure system tests for parry/guard dual-wire helpers (slice 6689).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/parryrate and internal/guardrate (0836/0843/6081/6211).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical

describe('parry/guard rate constants', function()
    it('pins clamps, pivots, and divisors', function()
        assert(phys.parryRateMin == 5)
        assert(phys.parryRateMax == 25)
        assert(phys.guardRateMin == 5)
        assert(phys.guardRateMax == 25)
        assert(phys.parrySkillDeltaPivot == 5)
        assert(phys.parrySkillDeltaOffset == 6)
        assert(phys.guardSkillDeltaPivot == 6)
        assert(phys.defenseRateBase == 10)
        assert(math.abs(phys.defenseLowBranchDivisor - 4) < 1e-12)
        assert(math.abs(phys.defenseHighBranchDivisor - (60 / 9)) < 1e-12)
    end)
end)

describe('canParryFromParams', function()
    it('requires facing, engaged, and no prevent-action', function()
        local base = {
            facing = true, engaged = true, isPC = true,
            parrySkillRank = 1, hasMainWeapon = true, mainSkillType = 3,
        }
        assert(phys.canParryFromParams(base))
        assert(not phys.canParryFromParams({
            facing = false, engaged = true, isPC = true,
            parrySkillRank = 1, hasMainWeapon = true, mainSkillType = 3,
        }))
        assert(not phys.canParryFromParams({
            facing = true, engaged = false, isPC = true,
            parrySkillRank = 1, hasMainWeapon = true, mainSkillType = 3,
        }))
        assert(not phys.canParryFromParams({
            facing = true, engaged = true, preventAction = true, isPC = true,
            parrySkillRank = 1, hasMainWeapon = true, mainSkillType = 3,
        }))
    end)

    it('PC needs parry rank, main weapon, and non-H2H skill', function()
        assert(phys.canParryFromParams({
            facing = true, engaged = true, isPC = true,
            parrySkillRank = 1, hasMainWeapon = true, mainSkillType = 3,
        }))
        assert(not phys.canParryFromParams({
            facing = true, engaged = true, isPC = true,
            parrySkillRank = 0, hasMainWeapon = true, mainSkillType = 3,
        }))
        assert(not phys.canParryFromParams({
            facing = true, engaged = true, isPC = true,
            parrySkillRank = 1, hasMainWeapon = false, mainSkillType = 3,
        }))
        assert(not phys.canParryFromParams({
            facing = true, engaged = true, isPC = true,
            parrySkillRank = 1, hasMainWeapon = true, mainSkillType = xi.skill.HAND_TO_HAND,
        }))
    end)

    it('non-PC needs CAN_PARRY mobmod', function()
        assert(phys.canParryFromParams({
            facing = true, engaged = true, isPC = false, canParryMobMod = 1,
        }))
        assert(not phys.canParryFromParams({
            facing = true, engaged = true, isPC = false, canParryMobMod = 0,
        }))
    end)
end)

describe('canGuardFromParams', function()
    it('requires facing, engaged, and no prevent-action', function()
        local base = {
            facing = true, engaged = true, isPC = true,
            guardSkillRank = 1, hasMainWeapon = true, mainSkillType = xi.skill.HAND_TO_HAND,
        }
        assert(phys.canGuardFromParams(base))
        assert(not phys.canGuardFromParams({
            facing = false, engaged = true, isPC = true,
            guardSkillRank = 1, hasMainWeapon = true, mainSkillType = xi.skill.HAND_TO_HAND,
        }))
        assert(not phys.canGuardFromParams({
            facing = true, engaged = true, preventAction = true, isPC = true,
            guardSkillRank = 1, hasMainWeapon = true, mainSkillType = xi.skill.HAND_TO_HAND,
        }))
    end)

    it('PC needs guard rank and bare hands or H2H main', function()
        assert(phys.canGuardFromParams({
            facing = true, engaged = true, isPC = true,
            guardSkillRank = 1, hasMainWeapon = false,
        }))
        assert(phys.canGuardFromParams({
            facing = true, engaged = true, isPC = true,
            guardSkillRank = 1, hasMainWeapon = true, mainSkillType = xi.skill.HAND_TO_HAND,
        }))
        assert(not phys.canGuardFromParams({
            facing = true, engaged = true, isPC = true,
            guardSkillRank = 1, hasMainWeapon = true, mainSkillType = 3,
        }))
        assert(not phys.canGuardFromParams({
            facing = true, engaged = true, isPC = true,
            guardSkillRank = 0, hasMainWeapon = false,
        }))
    end)

    it('mob/pet/trust needs MNK or PUP and CANNOT_GUARD == 0', function()
        assert(phys.canGuardFromParams({
            facing = true, engaged = true, isPC = false, isMobPetOrTrust = true,
            mainJob = xi.job.MNK, cannotGuardMod = 0,
        }))
        assert(phys.canGuardFromParams({
            facing = true, engaged = true, isPC = false, isMobPetOrTrust = true,
            mainJob = xi.job.PUP, cannotGuardMod = 0,
        }))
        assert(not phys.canGuardFromParams({
            facing = true, engaged = true, isPC = false, isMobPetOrTrust = true,
            mainJob = xi.job.WAR, cannotGuardMod = 0,
        }))
        assert(not phys.canGuardFromParams({
            facing = true, engaged = true, isPC = false, isMobPetOrTrust = true,
            mainJob = xi.job.MNK, cannotGuardMod = 1,
        }))
        assert(not phys.canGuardFromParams({
            facing = true, engaged = true, isPC = false, isMobPetOrTrust = false,
            mainJob = xi.job.MNK, cannotGuardMod = 0,
        }))
    end)
end)

describe('parryRateFromParams', function()
    it('follows skill-delta two-branch floor curve with clamp 5..25', function()
        -- attackerSkill fixed at 100; goldens match internal/parryrate
        assert(phys.parryRateFromParams({ defenderSkill = 82, attackerSkill = 100 }) == 5)   -- clamp floor
        assert(phys.parryRateFromParams({ defenderSkill = 86, attackerSkill = 100 }) == 5)
        assert(phys.parryRateFromParams({ defenderSkill = 90, attackerSkill = 100 }) == 6)
        assert(phys.parryRateFromParams({ defenderSkill = 100, attackerSkill = 100 }) == 8)  -- equal skills
        assert(phys.parryRateFromParams({ defenderSkill = 105, attackerSkill = 100 }) == 9)  -- low pivot
        assert(phys.parryRateFromParams({ defenderSkill = 106, attackerSkill = 100 }) == 10) -- high start
        assert(phys.parryRateFromParams({ defenderSkill = 166, attackerSkill = 100 }) == 19)
        assert(phys.parryRateFromParams({ defenderSkill = 206, attackerSkill = 100 }) == 25)
        assert(phys.parryRateFromParams({ defenderSkill = 300, attackerSkill = 100 }) == 25) -- clamp ceiling
    end)

    it('floors toward -inf on fractional intermediates', function()
        assert(phys.parryRateFromParams({ defenderSkill = 101, attackerSkill = 100 }) == 8)
        assert(phys.parryRateFromParams({ defenderSkill = 107, attackerSkill = 100 }) == 10)
        assert(phys.parryRateFromParams({ defenderSkill = 112, attackerSkill = 100 }) == 10)
        assert(phys.parryRateFromParams({ defenderSkill = 113, attackerSkill = 100 }) == 11)
    end)

    it('applies Issekigan and Inquartata after clamp', function()
        assert(phys.parryRateFromParams({
            defenderSkill = 300, attackerSkill = 100, issekiganPower = 25,
        }) == 50)
        assert(phys.parryRateFromParams({
            defenderSkill = 0, attackerSkill = 500, issekiganPower = 10,
        }) == 15)
        assert(phys.parryRateFromParams({
            defenderSkill = 100, attackerSkill = 100, inquartataMod = 5,
        }) == 13)
        assert(phys.parryRateFromParams({
            defenderSkill = 300, attackerSkill = 100, inquartataMod = 7,
        }) == 32)
        assert(phys.parryRateFromParams({
            defenderSkill = 400, attackerSkill = 0, issekiganPower = 25, inquartataMod = 5,
        }) == 55)
        assert(phys.parryRateFromParams({
            defenderSkill = 100, attackerSkill = 100, inquartataMod = -3,
        }) == 5)
    end)
end)

describe('guardRateFromParams', function()
    it('follows skill-delta two-branch floor curve pivot 6 without -6 offset', function()
        assert(phys.guardRateFromParams({ defenderSkill = 78, attackerSkill = 100 }) == 5)
        assert(phys.guardRateFromParams({ defenderSkill = 80, attackerSkill = 100 }) == 5)
        assert(phys.guardRateFromParams({ defenderSkill = 88, attackerSkill = 100 }) == 7)
        assert(phys.guardRateFromParams({ defenderSkill = 100, attackerSkill = 100 }) == 10) -- equal skills
        assert(phys.guardRateFromParams({ defenderSkill = 104, attackerSkill = 100 }) == 11)
        assert(phys.guardRateFromParams({ defenderSkill = 106, attackerSkill = 100 }) == 11) -- low pivot
        assert(phys.guardRateFromParams({ defenderSkill = 107, attackerSkill = 100 }) == 11) -- high start
        assert(phys.guardRateFromParams({ defenderSkill = 140, attackerSkill = 100 }) == 16)
        assert(phys.guardRateFromParams({ defenderSkill = 200, attackerSkill = 100 }) == 25)
        assert(phys.guardRateFromParams({ defenderSkill = 300, attackerSkill = 100 }) == 25)
    end)

    it('floors toward -inf on fractional intermediates', function()
        assert(phys.guardRateFromParams({ defenderSkill = 101, attackerSkill = 100 }) == 10)
        assert(phys.guardRateFromParams({ defenderSkill = 105, attackerSkill = 100 }) == 11)
        assert(phys.guardRateFromParams({ defenderSkill = 113, attackerSkill = 100 }) == 11)
        assert(phys.guardRateFromParams({ defenderSkill = 114, attackerSkill = 100 }) == 12)
        assert(phys.guardRateFromParams({ defenderSkill = 99, attackerSkill = 100 }) == 9)
    end)

    it('applies ADDITIVE_GUARD after clamp (Dodge over-cap)', function()
        assert(phys.guardRateFromParams({
            defenderSkill = 300, attackerSkill = 100, additiveGuard = 10,
        }) == 35)
        assert(phys.guardRateFromParams({
            defenderSkill = 0, attackerSkill = 500, additiveGuard = 8,
        }) == 13)
        assert(phys.guardRateFromParams({
            defenderSkill = 100, attackerSkill = 100, additiveGuard = 5,
        }) == 15)
        assert(phys.guardRateFromParams({
            defenderSkill = 100, attackerSkill = 100, additiveGuard = -7,
        }) == 3)
    end)

    it('is higher than parry at equal skills (no -6 offset)', function()
        assert(phys.guardRateFromParams({ defenderSkill = 100, attackerSkill = 100 }) == 10)
        assert(phys.parryRateFromParams({ defenderSkill = 100, attackerSkill = 100 }) == 8)
    end)
end)

describe('skill assembly injects', function()
    it('sums PC defender and attacker skill components', function()
        assert(phys.defenderParrySkillPC(100, 10, 5) == 115)
        assert(phys.defenderGuardSkillPC(80, 5, 10) == 95)
        assert(phys.attackerWeaponSkillPC(200, 15) == 215)
        assert(phys.defenderParrySkillPC() == 0)
        assert(phys.attackerWeaponSkillPC() == 0)
    end)
end)

describe('parrySucceeds and guardSucceeds', function()
    it('compares rate*100 to d10000 roll', function()
        assert(phys.parrySucceeds(20, 2000))
        assert(not phys.parrySucceeds(20, 2001))
        assert(phys.parrySucceeds(5, 500))
        assert(not phys.parrySucceeds(5, 501))
        assert(not phys.parrySucceeds(0, 1))
        assert(phys.parrySucceeds(100, 10000))
        assert(phys.parrySucceeds(50, 5000))
        assert(not phys.parrySucceeds(50, 5001))

        assert(phys.guardSucceeds(20, 2000))
        assert(not phys.guardSucceeds(20, 2001))
        assert(phys.guardSucceeds(35, 3500))
        assert(not phys.guardSucceeds(35, 3501))
        assert(not phys.guardSucceeds(0, 1))
        assert(phys.guardSucceeds(100, 10000))
    end)
end)
