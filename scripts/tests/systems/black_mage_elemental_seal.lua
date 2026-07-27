require('scripts/globals/job_utils/black_mage')

describe('Black Mage Elemental Seal', function()
    it('adds the fixed Elemental Seal effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.black_mage.useElementalSeal(player, {}, {})

        assert(result == xi.effect.ELEMENTAL_SEAL and effect.id == xi.effect.ELEMENTAL_SEAL)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
