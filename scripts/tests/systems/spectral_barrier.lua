require('scripts/actions/mobskills/spectral_barrier')
describe('Spectral Barrier mob skill', function()
    it('applies Magic Shield with random duration 30-60', function()
        local barrier = require('scripts/actions/mobskills/spectral_barrier')
        local buff = xi.mobskills.mobBuffMove
        local random = math.random
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        math.random = function(a, b)
            assert(a == 30 and b == 60)
            return 45
        end
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(barrier.onMobSkillCheck({}, {}, skill) == 0)
        assert(barrier.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.MAGIC_SHIELD)
        xi.mobskills.mobBuffMove = buff
        math.random = random
        assert(params[2] == xi.effect.MAGIC_SHIELD and params[3] == 1 and params[5] == 45 and message == 456)
    end)
end)
