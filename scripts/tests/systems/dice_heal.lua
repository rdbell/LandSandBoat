require('scripts/actions/mobskills/dice_heal')

describe('Dice Heal mob skill', function()
    it('restores exactly missing HP, wakes the target, and reports the heal', function()
        local restored, woke, message = nil, false, nil
        local target = {
            getMaxHP = function() return 1000 end,
            getHP = function() return 725 end,
            addHP = function(_, value) restored = value end,
            wakeUp = function() woke = true end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local diceHeal = require('scripts/actions/mobskills/dice_heal')

        assert(diceHeal.onMobSkillCheck(target, {}, skill) == 0)
        assert(diceHeal.onMobWeaponSkill({}, target, skill, {}) == 275)
        assert(restored == 275 and woke and message == xi.msg.basic.SELF_HEAL)
    end)

    it('still wakes a target already at full HP', function()
        local restored, woke, message = nil, false, nil
        local target = {
            getMaxHP = function() return 1000 end,
            getHP = function() return 1000 end,
            addHP = function(_, value) restored = value end,
            wakeUp = function() woke = true end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local diceHeal = require('scripts/actions/mobskills/dice_heal')

        assert(diceHeal.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(restored == 0 and woke and message == xi.msg.basic.SELF_HEAL)
    end)
end)
