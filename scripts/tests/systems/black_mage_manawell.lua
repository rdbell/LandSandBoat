require('scripts/globals/job_utils/black_mage')

describe('Black Mage Manawell', function()
    it('adds the fixed Manawell effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.black_mage.useManawell(player, target, {})

        assert(result == xi.effect.MANAWELL and effect.id == xi.effect.MANAWELL)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
