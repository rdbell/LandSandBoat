require('scripts/globals/job_utils/bard')

describe('Bard Nightingale', function()
    it('adds Nightingale without a power field and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.bard.useNightingale(player, {}, {})

        assert(result == xi.effect.NIGHTINGALE and effect.id == xi.effect.NIGHTINGALE)
        assert(effect.values.power == nil and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
