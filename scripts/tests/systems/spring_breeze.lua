require('scripts/actions/mobskills/spring_breeze')
describe('Spring Breeze mob skill', function()
    it('applies Sleep and halves TP', function()
        local breeze = require('scripts/actions/mobskills/spring_breeze')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message, tp = nil, nil, 1000
        local target = {
            getTP = function() return tp end,
            setTP = function(_, v) tp = v end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(breeze.onMobSkillCheck(target, {}, skill) == 0)
        assert(breeze.onMobWeaponSkill({}, target, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SLEEP_I and params[6] == 20 and tp == 500 and message == 456)
    end)
end)
