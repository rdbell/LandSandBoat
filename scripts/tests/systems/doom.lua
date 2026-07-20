require('scripts/actions/mobskills/doom')

describe('Doom mob skill', function()
    it('allows use, forwards its status-effect message, and returns Doom', function()
        local doom = require('scripts/actions/mobskills/doom')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 777
        end

        assert(doom.onMobSkillCheck({}, {}, {}) == 0)
        assert(doom.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DOOM)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(params[1] == xi.effect.DOOM and params[2] == 10 and params[3] == 3 and params[4] == 30)
        assert(message == 777)
    end)
end)
