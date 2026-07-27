require('scripts/globals/job_utils/bard')

describe('Bard Tenuto', function()
    it('adds Tenuto without a power field and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.bard.useTenuto(player, {}, {})

        assert(result == xi.effect.TENUTO and effect.id == xi.effect.TENUTO)
        assert(effect.values.power == nil and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
