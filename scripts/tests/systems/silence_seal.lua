require('scripts/actions/mobskills/silence_seal')
describe('Silence Seal mob skill', function()
    it('applies Silence status', function()
        local seal = require('scripts/actions/mobskills/silence_seal')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(seal.onMobSkillCheck({}, {}, skill) == 0)
        assert(seal.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SILENCE)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SILENCE and params[4] == 1 and params[6] == 60 and message == 456)
    end)
end)
