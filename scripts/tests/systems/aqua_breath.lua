require('scripts/actions/mobskills/aqua_breath')

describe('Aqua Breath mob skill', function()
    it('uses Water breath parameters and damages only after processing succeeds', function()
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {}
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, p)
            params = p
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        local skill = require('scripts/actions/mobskills/aqua_breath')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.10 and params.damageCap == 500 and params.bonusDamage == 100)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.WATER)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.WATER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.WATER)
    end)
end)
