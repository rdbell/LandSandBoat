require('scripts/actions/mobskills/marionette_dice_2')

describe('Marionette Dice 2 mob skill', function()
    it('caps its random HP restore at missing HP and reports the actual restore', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_2')
        local random = math.random
        local healed, message = nil, nil
        local mob, action = {}, {}
        local target = { getMaxHP = function() return 1000 end, getHP = function() return 700 end, addHP = function(_, value) healed = value end }
        local skill = { setMsg = function(_, value) message = value end }
        math.random = function(low, high) assert(low == 400 and high == 600); return 500 end

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == 300)
        assert(healed == 300 and message == xi.msg.basic.SELF_HEAL_SECONDARY)

        math.random = random
    end)
end)
