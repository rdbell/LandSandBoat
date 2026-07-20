require('scripts/actions/mobskills/tormentful_glare')
describe('Tormentful Glare mob skill', function()
    it('applies Curse I gaze for 360s', function()
        local skill = require('scripts/actions/mobskills/tormentful_glare')
        local gaze = xi.mobskills.mobGazeMove
        local message, gazeParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobGazeMove = function(...) gazeParams = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.CURSE_I and message == 456)
        xi.mobskills.mobGazeMove = gaze
        assert(gazeParams[3] == xi.effect.CURSE_I and gazeParams[4] == 30 and gazeParams[6] == 360)
    end)
end)
