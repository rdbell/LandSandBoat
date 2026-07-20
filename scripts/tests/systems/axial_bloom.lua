require('scripts/actions/mobskills/axial_bloom')
describe('Axial Bloom mob skill', function()
    it('applies Bind with random duration 30-60', function()
        local skill = require('scripts/actions/mobskills/axial_bloom')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local args, message = nil, nil
        math.random = function(a, b)
            assert(a == 30 and b == 60)
            return 45
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BIND)
        math.random = origRandom
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == xi.effect.BIND and args[2] == 1 and args[4] == 45)
        assert(message == 242)
    end)
end)
