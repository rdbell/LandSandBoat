require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Scarlet Delirium host', function()
    it('adds the self-origin Scarlet Delirium request from actor job points', function()
        local applied
        local player = {
            getJobPointLevel = function()
                return 20
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useScarletDelirium(player, {}, {}) == xi.effect.SCARLET_DELIRIUM)
        assert(applied.effect == xi.effect.SCARLET_DELIRIUM)
        assert(applied.params.duration == 110 and applied.params.origin == player)
    end)
end)
