describe('Fortifying Wail mob skill', function()
    it('uses the TP-scaled Protect buff and forwards its message', function()
        local wail = require('scripts/actions/mobskills/fortifying_wail')
        local buffMove = xi.mobskills.mobBuffMove
        local args, message, tp = nil, nil, 1500
        local mob = { getTP = function() return tp end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) args = { ... }; return 777 end
        assert(wail.onMobSkillCheck({}, mob, skill) == 0 and wail.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PROTECT)
        assert(args[1] == mob and args[2] == xi.effect.PROTECT and args[3] == 60 and args[4] == 0 and args[5] == 150 and message == 777)
        tp = 3000
        wail.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobBuffMove = buffMove
        assert(args[5] == 240)
    end)
end)
