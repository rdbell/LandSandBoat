require('scripts/globals/job_utils/samurai')

describe('Samurai Yaegasumi host', function()
    it('applies its fixed self-origin effect', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.samurai.useYaegasumi(player, player, {}) == xi.effect.YAEGASUMI)
        assert(applied.effect == xi.effect.YAEGASUMI)
        assert(applied.params.power == 12 and applied.params.duration == 45)
        assert(applied.params.origin == player)
    end)
end)
