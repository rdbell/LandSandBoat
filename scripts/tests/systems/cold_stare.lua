require('scripts/actions/mobskills/cold_stare')
describe('Cold Stare mob skill', function()
    it('applies gaze Silence', function()
        local skill = require('scripts/actions/mobskills/cold_stare')
        local gazeMove = xi.mobskills.mobGazeMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SILENCE)
        assert(status[1] == xi.effect.SILENCE and status[4] == 60 and msg == 242)
        xi.mobskills.mobGazeMove = gazeMove
    end)
end)
