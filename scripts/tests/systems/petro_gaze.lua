require('scripts/actions/mobskills/petro_gaze')

describe('Petro Gaze mob skill', function()
    it('always allows use and applies fixed Petrification via gaze', function()
        local skill = require('scripts/actions/mobskills/petro_gaze')
        local host = xi.mobskills.mobGazeMove
        local params, message = nil, nil
        local s = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobGazeMove = function(...)
            params = { ... }
            return 456
        end
        assert(skill.onMobSkillCheck({}, {}, s) == 0)
        assert(skill.onMobWeaponSkill({}, {}, s, {}) == xi.effect.PETRIFICATION)
        xi.mobskills.mobGazeMove = host
        assert(params[3] == xi.effect.PETRIFICATION and params[4] == 1 and params[5] == 0 and params[6] == 25)
        assert(message == 456)
    end)
end)
