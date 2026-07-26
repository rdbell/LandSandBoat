require('scripts/globals/job_utils/samurai')

describe('Samurai Hamanoha host', function()
    it('applies the target-calculated effect with actor origin', function()
        local applied
        local player = {}
        local target = {
            getJobPointLevel = function()
                return 20
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.samurai.useHamanoha(player, target, {}) == xi.effect.HAMANOHA)
        assert(applied.effect == xi.effect.HAMANOHA)
        assert(applied.params.power == 12 and applied.params.duration == 200 and applied.params.origin == player)
    end)
end)
