require('scripts/actions/mobskills/light_of_penance')

describe('Light of Penance mob skill', function()
    it('zeros TP, applies Blindness then Bind gaze effects, and reports TP reduction', function()
        local penance = require('scripts/actions/mobskills/light_of_penance')
        local gaze = xi.mobskills.mobGazeMove
        local tp, effects, message = nil, {}, nil
        local mob = {}
        local target = { setTP = function(_, value) tp = value end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobGazeMove = function(_, _, ...) effects[#effects + 1] = { ... } end

        assert(penance.onMobSkillCheck(target, mob, skill) == 0)
        assert(penance.onMobWeaponSkill(mob, target, skill, {}) == 0)

        xi.mobskills.mobGazeMove = gaze
        assert(tp == 0 and message == xi.msg.basic.TP_REDUCED)
        assert(#effects == 2)
        assert(effects[1][1] == xi.effect.BLINDNESS and effects[1][2] == 100 and effects[1][3] == 0 and effects[1][4] == 60)
        assert(effects[2][1] == xi.effect.BIND and effects[2][2] == 1 and effects[2][3] == 0 and effects[2][4] == 60)
    end)
end)
