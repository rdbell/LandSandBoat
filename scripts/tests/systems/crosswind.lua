require('scripts/actions/mobskills/crosswind')

describe('Crosswind mob skill', function()
    it('rejects non-1746 Wardens and allows other mobs', function()
        local crosswind = require('scripts/actions/mobskills/crosswind')
        local warden = { getSpecies = function() return xi.mobSpecies.WARDEN end, getModelId = function() return 1 end }
        local permittedWarden = { getSpecies = function() return xi.mobSpecies.WARDEN end, getModelId = function() return 1746 end }
        local other = { getSpecies = function() return 0 end, getModelId = function() return 1 end }

        assert(crosswind.onMobSkillCheck(nil, warden, nil) == 1)
        assert(crosswind.onMobSkillCheck(nil, permittedWarden, nil) == 0)
        assert(crosswind.onMobSkillCheck(nil, other, nil) == 0)
    end)

    it('uses fixed Wind breath parameters and damages only after processing succeeds', function()
        local crosswind = require('scripts/actions/mobskills/crosswind')
        local originalBreathMove = xi.mobskills.mobBreathMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {}
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end

        assert(crosswind.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.percentMultipier == 0.0833 and params.damageCap == 333 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.WIND)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(crosswind.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobBreathMove = originalBreathMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.WIND)
    end)
end)
