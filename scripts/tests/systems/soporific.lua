require('scripts/actions/mobskills/soporific')
describe('Soporific mob skill', function()
    it('applies Sleep I status', function()
        local sop = require('scripts/actions/mobskills/soporific')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(sop.onMobSkillCheck({}, {}, skill) == 0)
        assert(sop.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SLEEP_I and params[4] == 1 and params[6] == 30 and message == 456)
    end)
end)
