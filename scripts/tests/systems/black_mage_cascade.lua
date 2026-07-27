require('scripts/globals/job_utils/black_mage')

describe('Black Mage Cascade', function()
    it('adds the fixed Cascade effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.black_mage.useCascade(player, {}, {})

        assert(result == xi.effect.CASCADE and effect.id == xi.effect.CASCADE)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
