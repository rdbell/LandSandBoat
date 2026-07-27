require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Azure Lore', function()
    it('adds the fixed Azure Lore effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useAzureLore(player, {}, {}, {})

        assert(result == xi.effect.AZURE_LORE and effect.id == xi.effect.AZURE_LORE)
        assert(effect.values.power == 1 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
