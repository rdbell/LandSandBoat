require('scripts/actions/mobskills/dice_tp_loss')

describe('Dice TP Loss mob skill', function()
    it('reports no effect without changing zero TP', function()
        local setTP, message = nil, nil
        local target = {
            getTP = function() return 0 end,
            setTP = function(_, value) setTP = value end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local diceTPLoss = require('scripts/actions/mobskills/dice_tp_loss')

        assert(diceTPLoss.onMobSkillCheck(target, {}, skill) == 0)
        assert(diceTPLoss.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(setTP == nil and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)

    it('resets nonzero TP and reports the reduction', function()
        local setTP, message = nil, nil
        local target = {
            getTP = function() return 1000 end,
            setTP = function(_, value) setTP = value end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local diceTPLoss = require('scripts/actions/mobskills/dice_tp_loss')

        assert(diceTPLoss.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(setTP == 0 and message == xi.msg.basic.TP_REDUCED)
    end)
end)
