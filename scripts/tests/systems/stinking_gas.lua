require('scripts/actions/mobskills/stinking_gas')
describe('Stinking Gas mob skill', function()
    it('applies VIT Down status', function()
        local gas = require('scripts/actions/mobskills/stinking_gas')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(gas.onMobSkillCheck({}, {}, skill) == 0)
        assert(gas.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.VIT_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.VIT_DOWN and params[4] == 10 and params[5] == 5 and params[6] == 180 and message == 456)
    end)
end)
