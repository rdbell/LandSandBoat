require('scripts/actions/mobskills/bubble_shower')
describe('Bubble Shower mob skill', function()
    it('uses Water breath plan and STR_DOWN when processed', function()
        local skill = require('scripts/actions/mobskills/bubble_shower')
        local breathMove = xi.mobskills.mobBreathMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 40)
        assert(params.percentMultipier == 0.0625 and params.damageCap == 200)
        assert(params.element == xi.element.WATER and params.resistStat == xi.mod.INT)
        assert(status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 40)
        assert(damage[1] == 40 and status[1] == xi.effect.STR_DOWN and status[2] == 10 and status[3] == 9)
        xi.mobskills.mobBreathMove = breathMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
