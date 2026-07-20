require('scripts/actions/mobskills/marionette_dice_3')

describe('Marionette Dice 3 mob skill', function()
    it('caps its random MP restore at missing MP and reports the actual restore', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_3')
        local random = math.random
        local restored, message = nil, nil
        local mob, action = {}, {}
        local target = { getMaxMP = function() return 500 end, getMP = function() return 300 end, addMP = function(_, value) restored = value end }
        local skill = { setMsg = function(_, value) message = value end }
        math.random = function(low, high) assert(low == 200 and high == 300); return 250 end

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == 200)
        assert(restored == 200 and message == xi.msg.basic.RECOVERS_MP_SECONDARY)

        math.random = random
    end)
end)
