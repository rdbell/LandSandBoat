require('scripts/actions/mobskills/slumber_powder')
describe('Slumber Powder mob skill', function()
    it('applies status effect', function()
        local skillmod = require('scripts/actions/mobskills/slumber_powder')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skillmod.onMobSkillCheck({}, {}, skill) == 0)
        assert(skillmod.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SLEEP_I and params[6] == 30 and message == 456)
    end)
end)
