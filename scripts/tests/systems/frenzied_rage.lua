describe('Frenzied Rage mob skill', function()
    it('allows use, requests Attack Boost, and forwards its message', function()
        local rage = require('scripts/actions/mobskills/frenzied_rage')
        local buffMove = xi.mobskills.mobBuffMove
        local args, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) args = { ... }; return 777 end
        assert(rage.onMobSkillCheck({}, mob, skill) == 0 and rage.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.ATTACK_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(args[1] == mob and args[2] == xi.effect.ATTACK_BOOST and args[3] == 20 and args[4] == 0 and args[5] == 120 and message == 777)
    end)
end)
