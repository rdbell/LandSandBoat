require('scripts/actions/mobskills/antiphase')
describe('Antiphase mob skill', function()
    it('allows use, applies fixed Silence parameters, and forwards the host message', function()
        local effect = xi.mobskills.mobStatusEffectMove; local params, message = nil, nil
        local mob, target, skill = {}, {}, { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 91 end
        local antiphase = require('scripts/actions/mobskills/antiphase')
        assert(antiphase.onMobSkillCheck(target, mob, skill) == 0 and antiphase.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.SILENCE)
        xi.mobskills.mobStatusEffectMove = effect
        assert(params[3] == xi.effect.SILENCE and params[4] == 1 and params[5] == 0 and params[6] == 90 and message == 91)
    end)
end)
