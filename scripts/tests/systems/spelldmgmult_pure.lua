-----------------------------------
-- Pure system tests for spell damage product mult dual-wire (slice 6711).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/spelldmgmult (0862 / 6090).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local skill = xi.skill
local el = xi.element
local spell = xi.magic.spell
local spellGroup = xi.magic.spellGroup

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Spell dmg mult constants', function()
    it('pins fixed mults and clamps', function()
        assert(dmg.spellDmgDefaultMult == 1)
        assert(dmg.divineSealMult == 2)
        assert(almost(dmg.ebullienceBase, 1.2))
        assert(almost(dmg.undeadDivinePenalty, 1.5))
        assert(almost(dmg.mtdrMin, 0.4))
        assert(almost(dmg.mtdrMax, 1.0))
        assert(dmg.additionalResistTierRank == 4)
        assert(almost(dmg.additionalResistTierHalf, 0.5))
    end)
end)

describe('calculateMTDRFromParams', function()
    it('skips non-PC and single target', function()
        assert(dmg.calculateMTDRFromParams({ isPC = false, targetAmount = 5 }) == 1)
        assert(dmg.calculateMTDRFromParams({ isPC = true, targetAmount = 1 }) == 1)
    end)

    it('applies clamp ladder', function()
        -- 0.9 - 0.05*2 = 0.8
        assert(almost(dmg.calculateMTDRFromParams({ isPC = true, targetAmount = 2 }), 0.8))
        -- deep multi-target floors at 0.4
        assert(almost(dmg.calculateMTDRFromParams({ isPC = true, targetAmount = 20 }), 0.4))
    end)
end)

describe('staff and affinity FromParams', function()
    it('staff and affinity none and scale', function()
        assert(dmg.calculateElementalStaffBonusFromParams({ spellElement = el.NONE, staffMod = 3 }) == 1)
        assert(almost(dmg.calculateElementalStaffBonusFromParams({
            spellElement = el.FIRE, staffMod = 2,
        }), 1.10))
        assert(dmg.calculateElementalAffinityBonusFromParams({ spellElement = el.NONE, elementalMABMod = 50 }) == 1)
        assert(almost(dmg.calculateElementalAffinityBonusFromParams({
            spellElement = el.ICE, elementalMABMod = 25,
        }), 1.25))
    end)
end)

describe('calculateAdditionalResistTierFromParams', function()
    it('subtle sorcery and rank gates', function()
        assert(dmg.calculateAdditionalResistTierFromParams({
            hasSubtleSorcery = true, resistanceRank = 10,
        }) == 1)
        assert(dmg.calculateAdditionalResistTierFromParams({
            hasSubtleSorcery = false, resistanceRank = 3,
        }) == 1)
        assert(almost(dmg.calculateAdditionalResistTierFromParams({
            hasSubtleSorcery = false, resistanceRank = 4,
        }), 0.5))
    end)
end)

describe('Divine seal / emblem / elemental seal / ebullience', function()
    it('divine seal undead healing only', function()
        local m, c = dmg.calculateDivineSealMultiplierFromParams({})
        assert(m == 1 and not c)
        m, c = dmg.calculateDivineSealMultiplierFromParams({
            hasDivineSeal = true, targetIsUndead = true, skillType = skill.HEALING_MAGIC,
        })
        assert(m == 2 and c)
        m, c = dmg.calculateDivineSealMultiplierFromParams({
            hasDivineSeal = true, targetIsUndead = false, skillType = skill.HEALING_MAGIC,
        })
        assert(m == 1 and not c)
    end)

    it('divine emblem skill scale', function()
        local m, c = dmg.calculateDivineEmblemMultiplierFromParams({
            hasDivineEmblem = true, skillType = skill.DIVINE_MAGIC, divineSkill = 300,
        })
        assert(almost(m, 4) and c)
        m, c = dmg.calculateDivineEmblemMultiplierFromParams({
            hasDivineEmblem = true, skillType = skill.HEALING_MAGIC, divineSkill = 300,
        })
        assert(m == 1 and not c)
    end)

    it('elemental seal enhance mod', function()
        assert(almost(dmg.calculateEnhancedElementalSealMultiplierFromParams({
            hasElementalSeal = true, skillType = skill.ELEMENTAL_MAGIC,
            spellElement = el.FIRE, enhancesElementalSealMod = 20,
        }), 1.20))
        assert(dmg.calculateEnhancedElementalSealMultiplierFromParams({
            hasElementalSeal = true, skillType = skill.ELEMENTAL_MAGIC,
            spellElement = el.NONE, enhancesElementalSealMod = 20,
        }) == 1)
    end)

    it('ebullience black only', function()
        local m, c = dmg.calculateEbullienceMultiplierFromParams({
            hasEbullience = true, spellGroup = spellGroup.BLACK, ebullienceAmountMod = 10,
        })
        assert(almost(m, 1.30) and c)
        m, c = dmg.calculateEbullienceMultiplierFromParams({
            hasEbullience = true, spellGroup = spellGroup.WHITE, ebullienceAmountMod = 10,
        })
        assert(m == 1 and not c)
    end)
end)

describe('skill type / undead / helix / aoe / spell action', function()
    it('skill type settings powers', function()
        assert(almost(dmg.calculateSkillTypeMultiplierFromParams({
            skillType = skill.ELEMENTAL_MAGIC, elementalPower = 1.1,
        }), 1.1))
        assert(almost(dmg.calculateSkillTypeMultiplierFromParams({
            skillType = skill.DARK_MAGIC, darkPower = 0.9,
        }), 0.9))
        assert(dmg.calculateSkillTypeMultiplierFromParams({
            skillType = skill.HEALING_MAGIC, elementalPower = 2,
        }) == 1)
    end)

    it('undead divine penalty', function()
        assert(almost(dmg.calculateUndeadDivinePenaltyFromParams({
            targetIsUndead = true, skillType = skill.DIVINE_MAGIC,
        }), 1.5))
        assert(dmg.calculateUndeadDivinePenaltyFromParams({
            targetIsUndead = true, skillType = skill.ELEMENTAL_MAGIC,
        }) == 1)
    end)

    it('helix merit band and scale', function()
        assert(dmg.isHelixSpell(spell.GEOHELIX))
        assert(dmg.isHelixSpell(spell.LUMINOHELIX_II))
        assert(not dmg.isHelixSpell(spell.FLARE))
        assert(almost(dmg.calculateHelixMeritMultiplierFromParams({
            spellId = spell.GEOHELIX, helixMagicAccAttMerit = 10,
        }), 1.20))
        assert(dmg.calculateHelixMeritMultiplierFromParams({
            spellId = spell.FLARE, helixMagicAccAttMerit = 10,
        }) == 1)
    end)

    it('aoe resistance non-primary clamp', function()
        assert(dmg.calculateAreaOfEffectResistanceFromParams({
            isPrimaryTarget = true, dmgAoEMod = 5000,
        }) == 1)
        assert(almost(dmg.calculateAreaOfEffectResistanceFromParams({
            isPrimaryTarget = false, dmgAoEMod = 5000,
        }), 1.5))
        assert(almost(dmg.calculateAreaOfEffectResistanceFromParams({
            isPrimaryTarget = false, dmgAoEMod = 20000,
        }), 2.0))
    end)

    it('spell action type power mod', function()
        assert(almost(dmg.calculateSpellActionTypeMultiplierFromParams({
            powerMultiplierSpellMod = 25,
        }), 1.25))
        assert(dmg.calculateSpellActionTypeMultiplierFromParams({}) == 1)
    end)
end)
