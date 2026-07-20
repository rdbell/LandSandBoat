require('scripts/actions/mobskills/rabid_dance')
describe('Rabid Dance mob skill', function()
    it('always allows use and applies fixed Evasion Boost self-buff', function()
        local dance = require('scripts/actions/mobskills/rabid_dance')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end
        assert(dance.onMobSkillCheck({}, mob, skill) == 0)
        assert(dance.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.EVASION_BOOST and params[3] == 20 and params[4] == 0 and params[5] == 60)
        assert(message == 456)
    end)
end)
