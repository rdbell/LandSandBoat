require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Diabolic Eye host', function()
    it('adds the self-origin Diabolic Eye request from merit and enhancement inputs', function()
        local applied
        local player = {
            getMerit = function()
                return 4
            end,
            getMod = function()
                return 3
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useDiabolicEye(player, {}, {}, {}) == xi.effect.DIABOLIC_EYE)
        assert(applied.effect == xi.effect.DIABOLIC_EYE)
        assert(applied.params.power == 35 and applied.params.duration == 192 and applied.params.origin == player)
    end)
end)
