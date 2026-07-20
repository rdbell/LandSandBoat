require('scripts/actions/mobskills/terror_eye')
describe('Terror Eye mob skill', function()
    it('requires standing animation and applies Terror gaze', function()
        local skill = require('scripts/actions/mobskills/terror_eye')
        local gaze = xi.mobskills.mobGazeMove
        local message, gazeParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getAnimationSub = function() return 0 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getAnimationSub = function() return 4 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobGazeMove = function(...) gazeParams = { ... }; return 456 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.TERROR and message == 456)
        xi.mobskills.mobGazeMove = gaze
        assert(gazeParams[3] == xi.effect.TERROR and gazeParams[4] == 1 and gazeParams[6] == 30)
    end)
end)
