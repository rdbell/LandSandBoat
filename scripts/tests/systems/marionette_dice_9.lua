require('scripts/actions/mobskills/marionette_dice_9')

describe('Marionette Dice 9 mob skill', function()
    it('applies its Attack Boost and reports the gained effect', function()
        local dice = require('scripts/actions/mobskills/marionette_dice_9')
        local effect, message = nil, nil
        local mob, action = {}, {}
        local target = { addStatusEffect = function(_, value, options) effect = { value, options } end }
        local skill = { setMsg = function(_, value) message = value end }

        assert(dice.onMobSkillCheck(target, mob, skill) == 0)
        assert(dice.onMobWeaponSkill(mob, target, skill, action) == xi.effect.ATTACK_BOOST)
        assert(effect[1] == xi.effect.ATTACK_BOOST and effect[2].power == 25 and effect[2].duration == 30 and effect[2].origin == mob and message == xi.msg.basic.SKILL_GAIN_EFFECT)
    end)
end)
