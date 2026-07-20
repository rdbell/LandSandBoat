require('scripts/actions/mobskills/efflorescent_foetor')

describe('Efflorescent Foetor mob skill', function()
    it('applies random-duration Blindness then Silence and returns Blindness', function()
        local foetor = require('scripts/actions/mobskills/efflorescent_foetor')
        local statusMove, random = xi.mobskills.mobStatusEffectMove, math.random
        local requests, messages = {}, {}
        local skill = { setMsg = function(_, value) table.insert(messages, value) end }
        local durations = { 12, 29 }

        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(requests, { effect, power, tick, duration })
            return #requests * 100
        end
        math.random = function(low, high)
            assert(low == 10 and high == 30)
            return table.remove(durations, 1)
        end

        assert(foetor.onMobSkillCheck({}, {}, skill) == 0)
        assert(foetor.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BLINDNESS)

        xi.mobskills.mobStatusEffectMove, math.random = statusMove, random

        assert(requests[1][1] == xi.effect.BLINDNESS and requests[1][2] == 100 and requests[1][3] == 0 and requests[1][4] == 12)
        assert(requests[2][1] == xi.effect.SILENCE and requests[2][2] == 1 and requests[2][3] == 0 and requests[2][4] == 29)
        assert(messages[1] == 100 and messages[2] == 200)
    end)
end)
