require('scripts/actions/mobskills/nepenthean_hum')

describe('Nepenthean Hum mob skill', function()
    it('allows use only during hours 6-18 and applies Amnesia with subType 100', function()
        local hum = require('scripts/actions/mobskills/nepenthean_hum')
        local status = xi.mobskills.mobStatusEffectMove
        local hour, params, message = 12, nil, nil
        stub('VanadielHour', function() return hour end)
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end

        hour = 5
        assert(hum.onMobSkillCheck({}, {}, skill) == 1)
        hour = 6
        assert(hum.onMobSkillCheck({}, {}, skill) == 0)
        hour = 18
        assert(hum.onMobSkillCheck({}, {}, skill) == 0)
        hour = 19
        assert(hum.onMobSkillCheck({}, {}, skill) == 1)
        hour = 12
        assert(hum.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.AMNESIA)
        xi.mobskills.mobStatusEffectMove = status

        assert(params[3] == xi.effect.AMNESIA and params[4] == 1 and params[5] == 0 and params[6] == 60 and params[7] == 100)
        assert(message == 456)
    end)
end)
