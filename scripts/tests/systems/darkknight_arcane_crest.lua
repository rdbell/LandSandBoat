require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Arcane Crest host', function()
    it('adds the target request from actor job points without returning an effect', function()
        local applied
        local player = {
            getJobPointLevel = function()
                return 20
            end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useArcaneCrest(player, target, {}) == nil)
        assert(applied.effect == xi.effect.ARCANE_CREST)
        assert(applied.params.power == 20 and applied.params.duration == 200 and applied.params.origin == player)
    end)
end)
