require('scripts/globals/job_utils/ninja')

describe('Ninja Sange', function()
    it('adds the merit-derived Sange effect and returns its ID', function()
        local effect
        local player = {
            getMerit = function() return 4 end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ninja.useSange(player, {}, {}, {})

        assert(result == xi.effect.SANGE and effect.id == xi.effect.SANGE)
        assert(effect.values.power == 75 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
