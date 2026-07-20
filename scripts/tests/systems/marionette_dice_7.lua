require('scripts/actions/mobskills/marionette_dice_7')

describe('Marionette Dice 7 mob skill', function()
    it('reports uses and returns zero', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_7')
        local message = nil
        local mob, target, action = {}, {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == 0)
        assert(message == xi.msg.basic.USES)
    end)
end)
