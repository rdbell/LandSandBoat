require('scripts/actions/mobskills/petrifactive_breath')

describe('Petrifactive Breath mob skill', function()
    it('always allows use and applies fixed Petrification via status', function()
        local skill = require('scripts/actions/mobskills/petrifactive_breath')
        local host = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local s = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end
        assert(skill.onMobSkillCheck({}, {}, s) == 0)
        assert(skill.onMobWeaponSkill({}, {}, s, {}) == xi.effect.PETRIFICATION)
        xi.mobskills.mobStatusEffectMove = host
        assert(params[3] == xi.effect.PETRIFICATION and params[4] == 1 and params[5] == 0 and params[6] == 60)
        assert(message == 456)
    end)
end)
