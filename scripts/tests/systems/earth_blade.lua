require('scripts/actions/mobskills/earth_blade')

describe('Earth Blade mob skill', function()
    it('allows use, applies Enstone, and forwards its buff message', function()
        local blade = require('scripts/actions/mobskills/earth_blade')
        local buffMove = xi.mobskills.mobBuffMove
        local request, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration) request = { effect, power, tick, duration }; return 123 end
        assert(blade.onMobSkillCheck({}, {}, {}) == 0)
        assert(blade.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.ENSTONE)
        xi.mobskills.mobBuffMove = buffMove
        assert(request[1] == xi.effect.ENSTONE and request[2] == 65 and request[3] == 0 and request[4] == 30 and message == 123)
    end)
end)
