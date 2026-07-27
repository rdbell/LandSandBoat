require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Chain Affinity', function()
    it('adds the fixed Chain Affinity effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useChainAffinity(player, {}, {}, {})

        assert(result == xi.effect.CHAIN_AFFINITY and effect.id == xi.effect.CHAIN_AFFINITY)
        assert(effect.values.power == 1 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
