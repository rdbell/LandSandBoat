require('scripts/actions/mobskills/venom_storm')
describe('Venom Storm mob skill', function()
    it('applies random-power poison for 60s', function()
        local skill = require('scripts/actions/mobskills/venom_storm')
        local status, random = xi.mobskills.mobStatusEffectMove, math.random
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        math.random = function(a, b) assert(a == 20 and b == 30); return 25 end
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.POISON and message == 456)
        xi.mobskills.mobStatusEffectMove, math.random = status, random
        assert(params[3] == xi.effect.POISON and params[4] == 25 and params[5] == 3 and params[6] == 60)
    end)
end)
