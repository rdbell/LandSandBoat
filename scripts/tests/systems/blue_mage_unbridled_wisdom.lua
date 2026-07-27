require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Unbridled Wisdom', function()
    it('adds the fixed Unbridled Wisdom effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useUnbridledWisdom(player, target, {}, {})

        assert(result == xi.effect.UNBRIDLED_WISDOM and effect.id == xi.effect.UNBRIDLED_WISDOM)
        assert(effect.values.power == 16 and effect.values.duration == 30 and effect.values.tick == 1 and effect.values.origin == player)
    end)
end)
