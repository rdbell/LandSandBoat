describe('Jettatura mob skill', function()
    it('forwards its Terror gaze message and returns Terror', function()
        local jettatura = require('scripts/actions/mobskills/jettatura')
        local gaze = xi.mobskills.mobGazeMove
        local call, message
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobGazeMove = function(...) call = { ... }; return 777 end
        assert(jettatura.onMobSkillCheck(target, mob, skill) == 0 and jettatura.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.TERROR)
        xi.mobskills.mobGazeMove = gaze
        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.TERROR and call[4] == 1 and call[5] == 0 and call[6] == 10 and message == 777)
    end)
end)
