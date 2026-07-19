require('scripts/actions/mobskills/actinic_burst')

describe('Actinic Burst mob skill', function()
    it('allows use, applies fixed Flash parameters, and forwards the host message', function()
        local effect = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local mob, target, skill = {}, {}, { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 91
        end
        local burst = require('scripts/actions/mobskills/actinic_burst')
        assert(burst.onMobSkillCheck(target, mob, skill) == 0)
        assert(burst.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.FLASH)
        xi.mobskills.mobStatusEffectMove = effect
        assert(params[3] == xi.effect.FLASH and params[4] == 0 and params[5] == 0 and params[6] == 15)
        assert(message == 91)
    end)
end)
