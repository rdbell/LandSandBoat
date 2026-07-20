require('scripts/actions/mobskills/dream_flower')

describe('Dream Flower mob skill', function()
    it('uses a random Sleep I duration and forwards its status-effect message', function()
        local dreamFlower = require('scripts/actions/mobskills/dream_flower')
        local statusMove, random = xi.mobskills.mobStatusEffectMove, math.random
        local request, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            request = { effect, power, tick, duration }
            return 777
        end
        math.random = function(low, high)
            assert(low == 15 and high == 60)
            return 37
        end

        assert(dreamFlower.onMobSkillCheck({}, {}, {}) == 0)
        assert(dreamFlower.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove, math.random = statusMove, random
        assert(request[1] == xi.effect.SLEEP_I and request[2] == 1 and request[3] == 0 and request[4] == 37)
        assert(message == 777)
    end)
end)
