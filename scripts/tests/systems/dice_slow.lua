require('scripts/actions/mobskills/dice_slow')

describe('Dice Slow mob skill', function()
    it('requests Slow and Sleep and prioritizes Sleep when both succeed', function()
        local requests, message = {}, nil
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(requests, { effect, power, tick, duration })
            return true
        end
        local skill = { setMsg = function(_, value) message = value end }
        local diceSlow = require('scripts/actions/mobskills/dice_slow')

        assert(diceSlow.onMobSkillCheck({}, {}, {}) == 0)
        assert(diceSlow.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(requests[1][1] == xi.effect.SLOW and requests[1][2] == 1250 and requests[1][3] == 0 and requests[1][4] == 120)
        assert(requests[2][1] == xi.effect.SLEEP_I and requests[2][2] == 1 and requests[2][3] == 0 and requests[2][4] == 30)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)
    end)

    it('returns Slow when only Slow succeeds and misses when neither succeeds', function()
        local responses, message = { true, false }, nil
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function() return table.remove(responses, 1) end
        local skill = { setMsg = function(_, value) message = value end }
        local diceSlow = require('scripts/actions/mobskills/dice_slow')

        assert(diceSlow.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLOW)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)

        responses = { false, false }
        assert(diceSlow.onMobWeaponSkill({}, {}, skill, {}) == nil)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(message == xi.msg.basic.SKILL_MISS)
    end)
end)
