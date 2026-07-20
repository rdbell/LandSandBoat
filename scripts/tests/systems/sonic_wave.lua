require('scripts/actions/mobskills/sonic_wave')
describe('Sonic Wave mob skill', function()
    it('applies Defense Down status', function()
        local wave = require('scripts/actions/mobskills/sonic_wave')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(wave.onMobSkillCheck({}, {}, skill) == 0)
        assert(wave.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.DEFENSE_DOWN and params[4] == 40 and params[6] == 180 and message == 456)
    end)
end)
