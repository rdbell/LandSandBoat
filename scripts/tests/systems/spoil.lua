require('scripts/actions/mobskills/spoil')
describe('Spoil mob skill', function()
    it('applies STR Down status', function()
        local spoil = require('scripts/actions/mobskills/spoil')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(spoil.onMobSkillCheck({}, {}, skill) == 0)
        assert(spoil.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.STR_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.STR_DOWN and params[4] == 10 and params[5] == 3 and params[6] == 300 and message == 456)
    end)
end)
