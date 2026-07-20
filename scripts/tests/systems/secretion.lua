require('scripts/actions/mobskills/secretion')
describe('Secretion mob skill', function()
    it('applies Evasion Boost buff on target', function()
        local secretion = require('scripts/actions/mobskills/secretion')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(secretion.onMobSkillCheck({}, {}, skill) == 0)
        assert(secretion.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.EVASION_BOOST and params[3] == 25 and params[5] == 60 and message == 456)
    end)
end)
