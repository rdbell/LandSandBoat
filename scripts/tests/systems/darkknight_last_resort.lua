require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Last Resort host', function()
    it('adds the fixed self-origin Last Resort request', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useLastResort(player, {}, {}) == xi.effect.LAST_RESORT)
        assert(applied.effect == xi.effect.LAST_RESORT)
        assert(applied.params.duration == 180 and applied.params.power == nil and applied.params.origin == player)
    end)
end)
