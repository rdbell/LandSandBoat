require('scripts/actions/mobskills/rot_gas')
describe('Rot Gas mob skill', function()
    it('applies fixed Disease status', function()
        local gas = require('scripts/actions/mobskills/rot_gas')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(gas.onMobSkillCheck({}, {}, skill) == 0)
        assert(gas.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DISEASE)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.DISEASE and params[4] == 1 and params[6] == 360 and message == 456)
    end)
end)
