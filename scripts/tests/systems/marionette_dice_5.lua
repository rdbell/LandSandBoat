require('scripts/actions/mobskills/marionette_dice_5')

describe('Marionette Dice 5 mob skill', function()
    it('applies its Defense Boost and reports the gained effect', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_5')
        local effect, message = nil, nil
        local mob, action = {}, {}
        local target = { addStatusEffect = function(_, value, options) effect = { value, options } end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == xi.effect.DEFENSE_BOOST)
        assert(effect[1] == xi.effect.DEFENSE_BOOST and effect[2].power == 10 and effect[2].duration == 30 and effect[2].origin == mob and message == xi.msg.basic.SKILL_GAIN_EFFECT)
    end)
end)
