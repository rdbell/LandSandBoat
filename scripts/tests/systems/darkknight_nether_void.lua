require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Nether Void host', function()
    it('adds the self-origin Nether Void request from enhancement and job-point inputs', function()
        local applied
        local player = {
            getMod = function()
                return 7
            end,
            getJobPointLevel = function()
                return 4
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useNetherVoid(player, {}, {}) == xi.effect.NETHER_VOID)
        assert(applied.effect == xi.effect.NETHER_VOID)
        assert(applied.params.power == 65 and applied.params.duration == 60 and applied.params.origin == player)
    end)
end)
