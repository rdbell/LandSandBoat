require('scripts/globals/job_utils/samurai')

describe('Samurai Meditate host', function()
    it('applies the actor-calculated regain effect to the actor', function()
        local applied
        local player = {
            getMainJob = function()
                return xi.job.SAM
            end,
            getJobPointLevel = function()
                return 4
            end,
            getMod = function()
                return 10
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }
        local target = {
            addStatusEffect = function()
                error('Meditate must apply to the actor')
            end,
        }

        assert(xi.job_utils.samurai.useMeditate(player, target, {}) == xi.effect.MEDITATE)
        assert(applied.effect == xi.effect.MEDITATE)
        assert(applied.params.power == 40 and applied.params.duration == 25)
        assert(applied.params.origin == player and applied.params.tick == 3 and applied.params.icon == 0)
    end)
end)
