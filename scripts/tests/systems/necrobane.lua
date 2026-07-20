require('scripts/actions/mobskills/necrobane')

describe('Necrobane mob skill', function()
    it('applies Curse then a Paralysis gaze without returning a skill result', function()
        local bane = require('scripts/actions/mobskills/necrobane')
        local status, gaze = xi.mobskills.mobStatusEffectMove, xi.mobskills.mobGazeMove
        local curse, paralysis, message = nil, nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(...)
            curse = { ... }
            return 456
        end
        xi.mobskills.mobGazeMove = function(...)
            paralysis = { ... }
            return 789
        end

        assert(bane.onMobSkillCheck(target, mob, skill) == 0)
        assert(bane.onMobWeaponSkill(mob, target, skill, {}) == nil)
        xi.mobskills.mobStatusEffectMove, xi.mobskills.mobGazeMove = status, gaze

        assert(curse[1] == mob and curse[2] == target and curse[3] == xi.effect.CURSE_I)
        assert(curse[4] == 50 and curse[5] == 0 and curse[6] == 60)
        assert(paralysis[1] == mob and paralysis[2] == target and paralysis[3] == xi.effect.PARALYSIS)
        assert(paralysis[4] == 25 and paralysis[5] == 0 and paralysis[6] == 60)
        assert(message == 456)
    end)
end)
