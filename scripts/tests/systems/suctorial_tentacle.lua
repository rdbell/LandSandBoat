require('scripts/actions/mobskills/suctorial_tentacle')
describe('Suctorial Tentacle mob skill', function()
    it('applies Bind with random duration 30-90', function()
        local tent = require('scripts/actions/mobskills/suctorial_tentacle')
        local status = xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        math.random = function(a, b) assert(a == 30 and b == 90); return 60 end
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(tent.onMobSkillCheck({}, {}, skill) == 0)
        assert(tent.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BIND)
        xi.mobskills.mobStatusEffectMove = status
        math.random = random
        assert(params[3] == xi.effect.BIND and params[4] == 1 and params[6] == 60 and message == 456)
    end)
end)
