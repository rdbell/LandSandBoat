require('scripts/actions/mobskills/ululation')
describe('Ululation mob skill', function()
    it('applies random-power paralysis for 120s', function()
        local skill = require('scripts/actions/mobskills/ululation')
        local status, random = xi.mobskills.mobStatusEffectMove, math.random
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        math.random = function(a, b) assert(a == 18 and b == 22); return 20 end
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PARALYSIS and message == 456)
        xi.mobskills.mobStatusEffectMove, math.random = status, random
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 20 and params[6] == 120)
    end)
end)
