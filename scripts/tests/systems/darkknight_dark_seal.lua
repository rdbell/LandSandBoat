require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Dark Seal host', function()
    it('adds the self-origin Dark Seal request from merit and enhancement inputs', function()
        local applied
        local player = {
            getMerit = function()
                return 35
            end,
            getMod = function()
                return 3
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useDarkSeal(player, {}, {}) == xi.effect.DARK_SEAL)
        assert(applied.effect == xi.effect.DARK_SEAL)
        assert(applied.params.power == 25 and applied.params.duration == 60 and applied.params.subPower == 10.5)
        assert(applied.params.origin == player)
    end)
end)
