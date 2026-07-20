require('scripts/actions/mobskills/marionette_dice_15')

describe('Marionette Dice 15 mob skill', function()
    it('resets target recasts, reports abilities recharged, and returns one', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_15')
        local reset, message = false, nil
        local mob, action = {}, {}
        local target = { resetRecasts = function() reset = true end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == 1)
        assert(reset and message == xi.msg.basic.ABILITIES_RECHARGED)
    end)
end)
