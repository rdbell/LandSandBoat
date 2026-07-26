require('scripts/globals/job_utils/samurai')

describe('Samurai Sengikori host', function()
    it('applies the fixed self-origin effect', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.samurai.useSengikori(player, {}, {}) == xi.effect.SENGIKORI)
        assert(applied.effect == xi.effect.SENGIKORI)
        assert(applied.params.power == 25 and applied.params.duration == 60 and applied.params.origin == player)
    end)
end)
