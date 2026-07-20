require('scripts/actions/mobskills/disruptor')

describe('Disruptor mob skill', function()
    it('returns the dispelled effect and selects success or no-effect messages', function()
        local disruptor = require('scripts/actions/mobskills/disruptor')
        local message = nil
        local target = { dispelStatusEffect = function() return xi.effect.NONE end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(disruptor.onMobSkillCheck({}, {}, {}) == 0)
        assert(disruptor.onMobWeaponSkill({}, target, skill, {}) == xi.effect.NONE)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)

        target.dispelStatusEffect = function() return xi.effect.PROTECT end
        assert(disruptor.onMobWeaponSkill({}, target, skill, {}) == xi.effect.PROTECT)
        assert(message == xi.msg.basic.SKILL_ERASE)
    end)
end)
