require('scripts/actions/mobskills/obfuscate')

describe('Obfuscate mob skill', function()
    it('always allows use and applies Flash with sampled duration', function()
        local obfuscate = require('scripts/actions/mobskills/obfuscate')
        local status, random = xi.mobskills.mobStatusEffectMove, math.random
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end
        math.random = function(low, high)
            assert(low == 15 and high == 20)
            return 17
        end
        assert(obfuscate.onMobSkillCheck({}, {}, skill) == 0)
        assert(obfuscate.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.FLASH)
        xi.mobskills.mobStatusEffectMove, math.random = status, random
        assert(params[3] == xi.effect.FLASH and params[4] == 0 and params[5] == 0 and params[6] == 17)
        assert(message == 456)
    end)
end)
