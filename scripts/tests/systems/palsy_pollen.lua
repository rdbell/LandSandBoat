require('scripts/actions/mobskills/palsy_pollen')

describe('Palsy Pollen mob skill', function()
    it('always allows use and applies fixed Paralysis status', function()
        local pollen = require('scripts/actions/mobskills/palsy_pollen')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end
        assert(pollen.onMobSkillCheck({}, {}, skill) == 0)
        assert(pollen.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 30 and params[5] == 0 and params[6] == 60)
        assert(message == 456)
    end)
end)
