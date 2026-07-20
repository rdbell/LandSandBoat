require('scripts/actions/mobskills/sticky_thread')
describe('Sticky Thread mob skill', function()
    it('applies Slow with random duration 300-540', function()
        local thread = require('scripts/actions/mobskills/sticky_thread')
        local status = xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        math.random = function(a, b)
            assert(a == 300 and b == 540)
            return 400
        end
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(thread.onMobSkillCheck({}, {}, skill) == 0)
        assert(thread.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLOW)
        xi.mobskills.mobStatusEffectMove = status
        math.random = random
        assert(params[3] == xi.effect.SLOW and params[4] == 5000 and params[6] == 400 and message == 456)
    end)
end)
