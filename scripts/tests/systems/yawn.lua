require('scripts/actions/mobskills/yawn')
describe('Yawn mob skill', function()
    it('applies Sleep_I via gaze with random duration 60-120', function()
        local skill = require('scripts/actions/mobskills/yawn')
        local gazeMove = xi.mobskills.mobGazeMove
        local origRandom = math.random
        local args, message = nil, nil
        math.random = function(a, b)
            assert(a == 60 and b == 120)
            return 90
        end
        xi.mobskills.mobGazeMove = function(mob, target, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SLEEP_I)
        math.random = origRandom
        xi.mobskills.mobGazeMove = gazeMove
        assert(args[1] == xi.effect.SLEEP_I and args[2] == 1 and args[3] == 0 and args[4] == 90)
        assert(message == 242)
    end)
end)
