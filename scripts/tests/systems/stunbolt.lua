require('scripts/actions/mobskills/stunbolt')
describe('Stunbolt mob skill', function()
    it('applies Stun status for 10 seconds', function()
        local bolt = require('scripts/actions/mobskills/stunbolt')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(bolt.onMobSkillCheck({}, {}, skill) == 0)
        assert(bolt.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.STUN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.STUN and params[4] == 1 and params[6] == 10 and message == 456)
    end)
end)
