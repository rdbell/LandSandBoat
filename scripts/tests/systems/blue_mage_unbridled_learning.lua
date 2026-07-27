require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Unbridled Learning', function()
    it('adds the fixed Unbridled Learning effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useUnbridledLearning(player, target, {}, {})

        assert(result == xi.effect.UNBRIDLED_LEARNING and effect.id == xi.effect.UNBRIDLED_LEARNING)
        assert(effect.values.power == 16 and effect.values.duration == 60 and effect.values.tick == 1 and effect.values.origin == player)
    end)
end)
