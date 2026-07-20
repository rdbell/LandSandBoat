require('scripts/actions/mobskills/eternal_damnation')

describe('Eternal Damnation mob skill', function()
    it('allows use, forwards its Doom gaze message, and returns Doom', function()
        local eternalDamnation = require('scripts/actions/mobskills/eternal_damnation')
        local gazeMove = xi.mobskills.mobGazeMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 777
        end
        assert(eternalDamnation.onMobSkillCheck({}, {}, {}) == 0)
        assert(eternalDamnation.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DOOM)
        xi.mobskills.mobGazeMove = gazeMove
        assert(params[1] == xi.effect.DOOM and params[2] == 10 and params[3] == 3 and params[4] == 30)
        assert(message == 777)
    end)
end)
