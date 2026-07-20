require('scripts/actions/mobskills/quake_stomp')
describe('Quake Stomp mob skill', function()
    it('always allows use and applies fixed Boost self-buff', function()
        local stomp = require('scripts/actions/mobskills/quake_stomp')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        assert(stomp.onMobSkillCheck({}, mob, skill) == 0)
        assert(stomp.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.BOOST and params[3] == 100 and params[4] == 0 and params[5] == 60)
        assert(message == 456)
    end)
end)
