require('scripts/actions/mobskills/fantod')

describe('Fantod mob skill', function()
    it('allows use, requests its special Boost buff, and forwards its message', function()
        local fantod = require('scripts/actions/mobskills/fantod')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) buff = { ... }; return 777 end
        assert(fantod.onMobSkillCheck({}, mob, skill) == 0)
        assert(fantod.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == mob and buff[2] == xi.effect.BOOST and buff[3] == 400 and buff[4] == 0 and buff[5] == 180 and buff[7] == 1 and message == 777)
    end)
end)
