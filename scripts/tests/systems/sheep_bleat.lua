require('scripts/actions/mobskills/sheep_bleat')
describe('Sheep Bleat mob skill', function()
    it('rejects admission and applies Slow when used', function()
        local bleat = require('scripts/actions/mobskills/sheep_bleat')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        assert(bleat.onMobSkillCheck({}, {}, skill) == 1)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(bleat.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLOW)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SLOW and params[4] == 1000 and params[6] == 90 and message == 456)
    end)
end)
