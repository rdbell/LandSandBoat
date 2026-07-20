describe('Frost Blade mob skill', function()
    it('allows use, requests Enblizzard, and forwards its message', function()
        local blade = require('scripts/actions/mobskills/frost_blade')
        local buffMove = xi.mobskills.mobBuffMove
        local args, message = nil, nil
        local mob = {}; local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) args = { ... }; return 777 end
        assert(blade.onMobSkillCheck({}, mob, skill) == 0 and blade.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.ENBLIZZARD)
        xi.mobskills.mobBuffMove = buffMove
        assert(args[1] == mob and args[2] == xi.effect.ENBLIZZARD and args[3] == 65 and args[4] == 0 and args[5] == 30 and message == 777)
    end)
end)
