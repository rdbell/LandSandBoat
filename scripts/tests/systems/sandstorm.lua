require('scripts/actions/mobskills/sandstorm')
describe('Sandstorm mob skill', function()
    it('applies Blindness status', function()
        local storm = require('scripts/actions/mobskills/sandstorm')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(storm.onMobSkillCheck({}, {}, skill) == 0)
        assert(storm.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BLINDNESS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.BLINDNESS and params[4] == 30 and params[6] == 90 and message == 456)
    end)
end)
