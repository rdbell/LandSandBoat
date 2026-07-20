require('scripts/actions/mobskills/marionette_dice_6')

describe('Marionette Dice 6 mob skill', function()
    it('restores exactly the missing TP to 3000 and reports the increase', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_6')
        local restored, message = nil, nil
        local mob, action = {}, {}
        local target = { getTP = function() return 1250 end, addTP = function(_, value) restored = value end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == 1750)
        assert(restored == 1750 and message == xi.msg.basic.TP_INCREASE)
    end)
end)
