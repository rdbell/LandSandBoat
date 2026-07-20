require('scripts/actions/mobskills/stygian_vapor')
describe('Stygian Vapor mob skill', function()
    it('applies Plague status', function()
        local vapor = require('scripts/actions/mobskills/stygian_vapor')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(vapor.onMobSkillCheck({}, {}, skill) == 0)
        assert(vapor.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PLAGUE)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PLAGUE and params[4] == 5 and params[6] == 120 and message == 456)
    end)
end)
