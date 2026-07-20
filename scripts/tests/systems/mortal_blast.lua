require('scripts/actions/mobskills/Mortal_Blast')

describe('Mortal Blast mob skill', function()
    it('always allows use, applies KO gaze, and sets target HP to 0', function()
        local blast = require('scripts/actions/mobskills/Mortal_Blast')
        local gaze = xi.mobskills.mobGazeMove
        local params, message, hp = nil, nil, nil
        local mob = {}
        local target = { setHP = function(_, value) hp = value end }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobGazeMove = function(...)
            params = { ... }
            return 456
        end

        assert(blast.onMobSkillCheck(target, mob, skill) == 0)
        assert(blast.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.KO)
        xi.mobskills.mobGazeMove = gaze

        assert(params[1] == mob and params[2] == target and params[3] == xi.effect.KO)
        assert(params[4] == 1 and params[5] == 0 and params[6] == 1)
        assert(message == 456 and hp == 0)
    end)
end)
