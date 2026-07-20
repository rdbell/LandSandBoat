require('scripts/actions/mobskills/numbing_glare')

describe('Numbing Glare mob skill', function()
    it('always allows use and applies Paralysis gaze', function()
        local glare = require('scripts/actions/mobskills/numbing_glare')
        local gaze = xi.mobskills.mobGazeMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobGazeMove = function(...)
            params = { ... }
            return 456
        end
        assert(glare.onMobSkillCheck({}, {}, skill) == 0)
        assert(glare.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobGazeMove = gaze
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 25 and params[5] == 0 and params[6] == 180)
        assert(message == 456)
    end)
end)
