require('scripts/actions/mobskills/leafstorm_dispel')

describe('Leafstorm Dispel mob skill', function()
    it('applies Slow, dispels Dispelable and Food effects, and selects its message from the count', function()
        local leafstorm = require('scripts/actions/mobskills/leafstorm_dispel')
        local status = xi.mobskills.mobStatusEffectMove
        local slow, flags, message, count = nil, nil, nil, 0
        local target = { dispelAllStatusEffect = function(_, value) flags = value; return count end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            slow = { effect, power, tick, duration }
        end

        assert(leafstorm.onMobSkillCheck(target, {}, skill) == 0)
        assert(leafstorm.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(slow[1] == xi.effect.SLOW and slow[2] == 2500 and slow[3] == 0 and slow[4] == 120)
        assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)

        count = 3
        assert(leafstorm.onMobWeaponSkill({}, target, skill, {}) == 3)
        xi.mobskills.mobStatusEffectMove = status
        assert(message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
