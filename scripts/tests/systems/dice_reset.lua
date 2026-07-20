require('scripts/actions/mobskills/dice_reset')

describe('Dice Reset mob skill', function()
    it('is always available, resets recasts, and reports recharged abilities', function()
        local reset, message = false, nil
        local target = { resetRecasts = function() reset = true end }
        local skill = { setMsg = function(_, value) message = value end }
        local diceReset = require('scripts/actions/mobskills/dice_reset')

        assert(diceReset.onMobSkillCheck(target, {}, skill) == 0)
        assert(diceReset.onMobWeaponSkill({}, target, skill, {}) == 1)
        assert(reset and message == xi.msg.basic.ABILITIES_RECHARGED)
    end)
end)
