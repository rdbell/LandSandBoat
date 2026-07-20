require('scripts/actions/mobskills/torpid_glare')
describe('Torpid Glare mob skill', function()
    it('applies Sleep I gaze for 30s', function()
        local skill = require('scripts/actions/mobskills/torpid_glare')
        local gaze = xi.mobskills.mobGazeMove
        local message, gazeParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobGazeMove = function(...) gazeParams = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SLEEP_I and message == 456)
        xi.mobskills.mobGazeMove = gaze
        assert(gazeParams[3] == xi.effect.SLEEP_I and gazeParams[4] == 1 and gazeParams[6] == 30)
    end)
end)
