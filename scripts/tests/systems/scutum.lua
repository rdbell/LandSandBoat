require('scripts/actions/mobskills/scutum')
describe('Scutum mob skill', function()
    it('applies Defense Boost buff', function()
        local scutum = require('scripts/actions/mobskills/scutum')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(scutum.onMobSkillCheck({}, {}, skill) == 0)
        assert(scutum.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.DEFENSE_BOOST and params[3] == 70 and params[5] == 180 and message == 456)
    end)
end)
