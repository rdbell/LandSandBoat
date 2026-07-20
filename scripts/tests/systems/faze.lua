describe('Faze mob skill', function()
    it('allows use, forwards its Terror gaze message, and returns Terror', function()
        local faze = require('scripts/actions/mobskills/faze')
        local gazeMove = xi.mobskills.mobGazeMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobGazeMove = function(_, _, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 777
        end
        assert(faze.onMobSkillCheck({}, {}, {}) == 0)
        assert(faze.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.TERROR)
        xi.mobskills.mobGazeMove = gazeMove
        assert(params[1] == xi.effect.TERROR and params[2] == 1 and params[3] == 0 and params[4] == 10)
        assert(message == 777)
    end)
end)
