require('scripts/actions/mobskills/sandspray')
describe('Sandspray mob skill', function()
    it('applies Blindness status', function()
        local spray = require('scripts/actions/mobskills/sandspray')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(spray.onMobSkillCheck({}, {}, skill) == 0)
        assert(spray.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BLINDNESS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.BLINDNESS and params[4] == 25 and params[6] == 150 and message == 456)
    end)
end)
