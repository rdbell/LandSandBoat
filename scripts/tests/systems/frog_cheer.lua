describe('Frog Cheer mob skill', function()
    it('buffs the target, grants Elemental Seal from the mob, and forwards the buff message', function()
        local cheer = require('scripts/actions/mobskills/frog_cheer')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, seal, message = nil, nil, nil
        local mob = {}
        local target = { addStatusEffect = function(_, effect, options) seal = { effect, options } end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) buff = { ... }; return 777 end
        assert(cheer.onMobSkillCheck(target, mob, skill) == 0 and cheer.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.MAGIC_ATK_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == target and buff[2] == xi.effect.MAGIC_ATK_BOOST and buff[3] == 25 and buff[4] == 0 and buff[5] == 300 and seal[1] == xi.effect.ELEMENTAL_SEAL and seal[2].power == 1 and seal[2].duration == 60 and seal[2].origin == mob and message == 777)
    end)
end)
