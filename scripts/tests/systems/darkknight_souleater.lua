require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Souleater host', function()
    it('uses target job inputs for a self-origin Souleater request', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }
        local target = {
            getJobPointLevel = function()
                return 20
            end,
            getMod = function()
                return 3
            end,
            getMerit = function()
                return 35
            end,
        }

        assert(xi.job_utils.dark_knight.useSouleater(player, target, {}) == xi.effect.SOULEATER)
        assert(applied.effect == xi.effect.SOULEATER)
        assert(applied.params.power == 1 and applied.params.duration == 80 and applied.params.subPower == 10.5)
        assert(applied.params.origin == player)
    end)
end)
