describe('Kibosh mob skill', function()
    it('forwards its Amnesia status message and returns Amnesia', function()
        local kibosh = require('scripts/actions/mobskills/kibosh')
        local status = xi.mobskills.mobStatusEffectMove
        local call, message
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) call = { ... }; return 777 end
        assert(kibosh.onMobSkillCheck(target, mob, skill) == 0 and kibosh.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.AMNESIA)
        xi.mobskills.mobStatusEffectMove = status
        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.AMNESIA and call[4] == 1 and call[5] == 0 and call[6] == 60 and message == 777)
    end)
end)
