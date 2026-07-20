require('scripts/actions/mobskills/wind_breath')
describe('Wind Breath mob skill', function()
    it('uses its Wind breath plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/wind_breath')
        local breathMove = xi.mobskills.mobBreathMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.BREATH, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 150)
        assert(params.percentMultipier == 0.10 and params.damageCap == 400 and params.bonusDamage == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.WIND)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 150)
        xi.mobskills.mobBreathMove = breathMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 150 and damage[3] == xi.attackType.BREATH)
    end)
end)
