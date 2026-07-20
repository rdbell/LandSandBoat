require('scripts/actions/mobskills/mucus_spread')

describe('Mucus Spread mob skill', function()
    it('always allows use and applies its fixed Slow status', function()
        local mucus = require('scripts/actions/mobskills/mucus_spread')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end

        assert(mucus.onMobSkillCheck(target, mob, skill) == 0)
        assert(mucus.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.SLOW)
        xi.mobskills.mobStatusEffectMove = status

        assert(params[1] == mob and params[2] == target and params[3] == xi.effect.SLOW)
        assert(params[4] == 2500 and params[5] == 0 and params[6] == 90)
        assert(message == 456)
    end)
end)
