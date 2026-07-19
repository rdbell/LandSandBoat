require('scripts/actions/mobskills/benediction')

describe('Benediction mob skill', function()
    it('clears effects, restores exactly missing HP, wakes the target, and reports the heal', function()
        local benediction = require('scripts/actions/mobskills/benediction')
        local effectsErased, restored, woke, message = false, nil, false, nil
        local target = {
            eraseAllStatusEffect = function() effectsErased = true end,
            getMaxHP = function() return 1000 end,
            getHP = function() return 725 end,
            addHP = function(_, value) restored = value end,
            wakeUp = function() woke = true end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(benediction.onMobSkillCheck(target, {}, skill) == 0)
        assert(benediction.onMobWeaponSkill({}, target, skill, {}) == 275)
        assert(effectsErased and restored == 275 and woke and message == xi.msg.basic.SELF_HEAL)
    end)

    it('still clears effects and wakes a target already at full HP', function()
        local benediction = require('scripts/actions/mobskills/benediction')
        local effectsErased, restored, woke, message = false, nil, false, nil
        local target = {
            eraseAllStatusEffect = function() effectsErased = true end,
            getMaxHP = function() return 1000 end,
            getHP = function() return 1000 end,
            addHP = function(_, value) restored = value end,
            wakeUp = function() woke = true end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(benediction.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(effectsErased and restored == 0 and woke and message == xi.msg.basic.SELF_HEAL)
    end)
end)
