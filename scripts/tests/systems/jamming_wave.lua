describe('Jamming Wave mob skill', function()
    it('forwards its Silence status message and returns Silence', function()
        local wave = require('scripts/actions/mobskills/jamming_wave')
        local status = xi.mobskills.mobStatusEffectMove
        local call, message
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) call = { ... }; return 777 end
        assert(wave.onMobSkillCheck(target, mob, skill) == 0 and wave.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.SILENCE)
        xi.mobskills.mobStatusEffectMove = status
        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.SILENCE and call[4] == 1 and call[5] == 0 and call[6] == 120 and message == 777)
    end)
end)
