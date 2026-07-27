require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Efflux', function()
    it('adds the fixed Efflux effect with its tick and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useEfflux(player, {}, {}, {})

        assert(result == xi.effect.EFFLUX and effect.id == xi.effect.EFFLUX)
        assert(effect.values.power == 16 and effect.values.duration == 60 and effect.values.tick == 1 and effect.values.origin == player)
    end)
end)
