require('scripts/actions/mobskills/sand_veil')
describe('Sand Veil mob skill', function()
    it('applies Evasion Boost buff', function()
        local veil = require('scripts/actions/mobskills/sand_veil')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(veil.onMobSkillCheck({}, {}, skill) == 0)
        assert(veil.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.EVASION_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.EVASION_BOOST and params[3] == 100 and params[5] == 120 and message == 456)
    end)
end)
