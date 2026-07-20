describe('Frost Armor mob skill', function()
    it('allows use, requests Ice Spikes, and forwards its message', function()
        local armor = require('scripts/actions/mobskills/frost_armor')
        local buffMove = xi.mobskills.mobBuffMove
        local args, message = nil, nil
        local mob = {}; local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) args = { ... }; return 777 end
        assert(armor.onMobSkillCheck({}, mob, skill) == 0 and armor.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.ICE_SPIKES)
        xi.mobskills.mobBuffMove = buffMove
        assert(args[1] == mob and args[2] == xi.effect.ICE_SPIKES and args[3] == 10 and args[4] == 0 and args[5] == 180 and message == 777)
    end)
end)
