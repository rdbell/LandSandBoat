require('scripts/actions/mobskills/noisome_powder')

describe('Noisome Powder mob skill', function()
    it('allows use only during hours 6-18 and applies Attack Down', function()
        local powder = require('scripts/actions/mobskills/noisome_powder')
        local status = xi.mobskills.mobStatusEffectMove
        local hour, params, message = 12, nil, nil
        stub('VanadielHour', function() return hour end)
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end

        hour = 5
        assert(powder.onMobSkillCheck({}, {}, skill) == 1)
        hour = 6
        assert(powder.onMobSkillCheck({}, {}, skill) == 0)
        hour = 18
        assert(powder.onMobSkillCheck({}, {}, skill) == 0)
        hour = 19
        assert(powder.onMobSkillCheck({}, {}, skill) == 1)
        hour = 12
        assert(powder.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.ATTACK_DOWN)
        xi.mobskills.mobStatusEffectMove = status

        assert(params[3] == xi.effect.ATTACK_DOWN and params[4] == 40 and params[5] == 0 and params[6] == 120)
        assert(message == 456)
    end)
end)
