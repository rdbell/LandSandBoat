require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Convergence', function()
    it('adds the fixed Convergence effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useConvergence(player, {}, {}, {})

        assert(result == xi.effect.CONVERGENCE and effect.id == xi.effect.CONVERGENCE)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
