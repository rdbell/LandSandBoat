require('scripts/actions/mobskills/numbing_noise')

describe('Numbing Noise mob skill', function()
    it('always allows use and applies Stun', function()
        local noise = require('scripts/actions/mobskills/numbing_noise')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end
        assert(noise.onMobSkillCheck({}, {}, skill) == 0)
        assert(noise.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.STUN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.STUN and params[4] == 1 and params[5] == 0 and params[6] == 5)
        assert(message == 456)
    end)
end)
