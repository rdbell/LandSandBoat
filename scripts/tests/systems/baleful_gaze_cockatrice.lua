require('scripts/actions/mobskills/baleful_gaze_cockatrice')
describe('Baleful Gaze Cockatrice mob skill', function()
    it('applies Petrification via gaze for 120s', function()
        local skill = require('scripts/actions/mobskills/baleful_gaze_cockatrice')
        local gazeMove = xi.mobskills.mobGazeMove
        local args, message = nil, nil
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            args = { effect, power, tick, duration }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PETRIFICATION)
        xi.mobskills.mobGazeMove = gazeMove
        assert(args[1] == xi.effect.PETRIFICATION and args[2] == 1 and args[4] == 120)
        assert(message == 242)
    end)
end)
