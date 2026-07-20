require('scripts/actions/mobskills/paralysis_shower')

describe('Paralysis Shower mob skill', function()
    it('always allows use and applies fixed Paralysis status', function()
        local shower = require('scripts/actions/mobskills/paralysis_shower')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end
        assert(shower.onMobSkillCheck({}, {}, skill) == 0)
        assert(shower.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 20 and params[5] == 0 and params[6] == 120)
        assert(message == 456)
    end)
end)
