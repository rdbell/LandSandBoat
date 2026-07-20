describe('Fluorescence mob skill', function()
    it('allows use, requests its five-second Boost buff, and forwards its message', function()
        local fluorescence = require('scripts/actions/mobskills/fluorescence')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) buff = { ... }; return 777 end
        assert(fluorescence.onMobSkillCheck({}, mob, skill) == 0)
        assert(fluorescence.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == mob and buff[2] == xi.effect.BOOST and buff[3] == 400 and buff[4] == 0 and buff[5] == 5 and message == 777)
    end)
end)
