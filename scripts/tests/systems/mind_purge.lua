require('scripts/actions/mobskills/mind_purge')

describe('Mind Purge mob skill', function()
    it('dispels Dispelable and Food effects and selects its message from the count', function()
        local purge = require('scripts/actions/mobskills/mind_purge')
        local flags, message, count = nil, nil, 0
        local target = {
            dispelAllStatusEffect = function(_, value)
                flags = value
                return count
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(purge.onMobSkillCheck(target, {}, skill) == 0)
        assert(purge.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)

        count = 3
        assert(purge.onMobWeaponSkill({}, target, skill, {}) == 3)
        assert(message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
