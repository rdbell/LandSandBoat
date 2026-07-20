describe('Ion Efflux mob skill', function()
    it('forwards its Paralysis status message and returns Paralysis', function()
        local efflux = require('scripts/actions/mobskills/ion_efflux')
        local status = xi.mobskills.mobStatusEffectMove
        local call, message
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(...)
            call = { ... }
            return 777
        end

        assert(efflux.onMobSkillCheck(target, mob, skill) == 0 and efflux.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = status

        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.PARALYSIS and call[4] == 20 and call[5] == 0 and call[6] == 180 and message == 777)
    end)
end)
