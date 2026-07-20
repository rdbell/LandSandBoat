require('scripts/actions/mobskills/mind_break')

describe('Mind Break mob skill', function()
    it('always allows use and applies its Max MP Down gaze', function()
        local mind = require('scripts/actions/mobskills/mind_break')
        local gaze = xi.mobskills.mobGazeMove
        local params, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobGazeMove = function(...)
            params = { ... }
            return 456
        end

        assert(mind.onMobSkillCheck(target, mob, skill) == 0)
        assert(mind.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.MAX_MP_DOWN)
        xi.mobskills.mobGazeMove = gaze

        assert(params[1] == mob and params[2] == target and params[3] == xi.effect.MAX_MP_DOWN)
        assert(params[4] == 50 and params[5] == 0 and params[6] == 10)
        assert(message == 456)
    end)
end)
