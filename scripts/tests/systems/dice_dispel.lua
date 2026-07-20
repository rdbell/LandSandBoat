require('scripts/actions/mobskills/dice_dispel')

describe('Dice Dispel mob skill', function()
    it('always makes three dispel attempts and reports the number removed', function()
        local diceDispel = require('scripts/actions/mobskills/dice_dispel')
        local effects, calls, message = { xi.effect.NONE, 42, 43 }, 0, nil
        local target = {
            dispelStatusEffect = function()
                calls = calls + 1
                return effects[calls]
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(diceDispel.onMobSkillCheck(target, {}, skill) == 0)
        assert(diceDispel.onMobWeaponSkill({}, target, skill, {}) == 2)
        assert(calls == 3 and message == xi.msg.basic.DISAPPEAR_NUM)
    end)

    it('reports no effect when all three attempts fail', function()
        local calls, message = 0, nil
        local target = {
            dispelStatusEffect = function()
                calls = calls + 1
                return xi.effect.NONE
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local diceDispel = require('scripts/actions/mobskills/dice_dispel')

        assert(diceDispel.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(calls == 3 and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
