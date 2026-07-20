require('scripts/actions/mobskills/dissipation')

describe('Dissipation mob skill', function()
    it('applies Terror, dispels all effects, and selects its message from the count', function()
        local dissipation = require('scripts/actions/mobskills/dissipation')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local terror, message, count = nil, nil, 0
        local target = { dispelAllStatusEffect = function() return count end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            terror = { effect, power, tick, duration }
        end

        assert(dissipation.onMobSkillCheck({}, {}, {}) == 0)
        assert(dissipation.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(terror[1] == xi.effect.TERROR and terror[2] == 1 and terror[3] == 0 and terror[4] == 10)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)

        count = 3
        assert(dissipation.onMobWeaponSkill({}, target, skill, {}) == 3)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
