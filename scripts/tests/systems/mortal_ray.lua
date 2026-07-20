require('scripts/actions/mobskills/mortal_ray')

describe('Mortal Ray mob skill', function()
    it('always allows use and applies its Doom gaze', function()
        local ray = require('scripts/actions/mobskills/mortal_ray')
        local gaze = xi.mobskills.mobGazeMove
        local params, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobGazeMove = function(...)
            params = { ... }
            return 456
        end

        assert(ray.onMobSkillCheck(target, mob, skill) == 0)
        assert(ray.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.DOOM)
        xi.mobskills.mobGazeMove = gaze

        assert(params[1] == mob and params[2] == target and params[3] == xi.effect.DOOM)
        assert(params[4] == 10 and params[5] == 3 and params[6] == 45)
        assert(message == 456)
    end)
end)
