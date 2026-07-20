describe('Filamented Hold mob skill', function()
    it('allows use, forwards its Slow message, and returns Slow', function()
        local filamentedHold = require('scripts/actions/mobskills/filamented_hold')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 777
        end
        assert(filamentedHold.onMobSkillCheck({}, {}, {}) == 0)
        assert(filamentedHold.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLOW)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(params[1] == xi.effect.SLOW and params[2] == 2500 and params[3] == 0 and params[4] == 120)
        assert(message == 777)
    end)
end)
