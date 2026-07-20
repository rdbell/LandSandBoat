require('scripts/actions/mobskills/chthonian_ray')
describe('Chthonian Ray mob skill', function()
    it('applies gaze Doom', function()
        local skill = require('scripts/actions/mobskills/chthonian_ray')
        local gazeMove = xi.mobskills.mobGazeMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.DOOM)
        assert(status[1] == xi.effect.DOOM and status[2] == 10 and status[3] == 3 and status[4] == 30)
        assert(msg == 242)
        xi.mobskills.mobGazeMove = gazeMove
    end)
end)
