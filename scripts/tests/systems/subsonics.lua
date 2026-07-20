require('scripts/actions/mobskills/subsonics')
describe('Subsonics mob skill', function()
    it('applies Defense Down status', function()
        local sub = require('scripts/actions/mobskills/subsonics')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(sub.onMobSkillCheck({}, {}, skill) == 0)
        assert(sub.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.DEFENSE_DOWN and params[4] == 25 and params[6] == 180 and message == 456)
    end)
end)
