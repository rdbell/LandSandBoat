require('scripts/actions/mobskills/scream')
describe('Scream mob skill', function()
    it('applies MND Down status', function()
        local scream = require('scripts/actions/mobskills/scream')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(scream.onMobSkillCheck({}, {}, skill) == 0)
        assert(scream.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.MND_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.MND_DOWN and params[4] == 10 and params[5] == 3 and params[6] == 180 and message == 456)
    end)
end)
