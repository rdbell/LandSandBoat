require('scripts/actions/mobskills/awful_eye')
describe('Awful Eye mob skill', function()
    it('applies STR Down via gaze power 10 tick 18 duration 180', function()
        local skill = require('scripts/actions/mobskills/awful_eye')
        local gazeMove = xi.mobskills.mobGazeMove
        local args, message = nil, nil
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.STR_DOWN)
        xi.mobskills.mobGazeMove = gazeMove
        assert(args[1] == xi.effect.STR_DOWN and args[2] == 10 and args[3] == 18 and args[4] == 180)
        assert(message == 242)
    end)
end)
