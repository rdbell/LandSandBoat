require('scripts/actions/mobskills/blank_gaze')
describe('Blank Gaze mob skill', function()
    it('applies Paralysis via gaze power 35 for 180s', function()
        local skill = require('scripts/actions/mobskills/blank_gaze')
        local gazeMove = xi.mobskills.mobGazeMove
        local args, message = nil, nil
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobGazeMove = gazeMove
        assert(args[1] == xi.effect.PARALYSIS and args[2] == 35 and args[4] == 180)
        assert(message == 242)
    end)
end)
