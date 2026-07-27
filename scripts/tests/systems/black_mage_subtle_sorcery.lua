require('scripts/globals/job_utils/black_mage')

describe('Black Mage Subtle Sorcery', function()
    it('adds the fixed Subtle Sorcery effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.black_mage.useSubtleSorcery(player, {}, {})

        assert(result == xi.effect.SUBTLE_SORCERY and effect.id == xi.effect.SUBTLE_SORCERY)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
