describe('Intimidate mob skill', function()
    it('forwards its Slow gaze message and returns Slow', function()
        local intimidate = require('scripts/actions/mobskills/intimidate')
        local gaze = xi.mobskills.mobGazeMove
        local call, message
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobGazeMove = function(...)
            call = { ... }
            return 777
        end

        assert(intimidate.onMobSkillCheck(target, mob, skill) == 0 and intimidate.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.SLOW)
        xi.mobskills.mobGazeMove = gaze

        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.SLOW and call[4] == 1250 and call[5] == 0 and call[6] == 120 and message == 777)
    end)
end)
