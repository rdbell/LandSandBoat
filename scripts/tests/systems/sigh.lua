require('scripts/actions/mobskills/sigh')
describe('Sigh mob skill', function()
    it('applies Evasion Boost buff', function()
        local sigh = require('scripts/actions/mobskills/sigh')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(sigh.onMobSkillCheck({}, {}, skill) == 0)
        assert(sigh.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.EVASION_BOOST and params[3] == 200 and params[5] == 15 and message == 456)
    end)
end)
