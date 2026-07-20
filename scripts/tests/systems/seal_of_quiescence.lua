require('scripts/actions/mobskills/seal_of_quiescence')
describe('Seal of Quiescence mob skill', function()
    it('applies Silence status but returns Mute', function()
        local seal = require('scripts/actions/mobskills/seal_of_quiescence')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(seal.onMobSkillCheck({}, {}, skill) == 0)
        assert(seal.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.MUTE)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SILENCE and params[4] == 30 and params[6] == 75 and message == 456)
    end)
end)
