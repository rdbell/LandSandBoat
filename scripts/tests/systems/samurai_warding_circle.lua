require('scripts/globals/job_utils/samurai')

describe('Samurai Warding Circle host', function()
    it('applies the actor-calculated effect to its target', function()
        local applied
        local player = {
            getMainJob = function()
                return xi.job.SAM
            end,
            getMod = function(_, mod)
                if mod == xi.mod.WARDING_CIRCLE_POTENCY then
                    return 2
                end
                if mod == xi.mod.WARDING_CIRCLE_DURATION then
                    return 30
                end
                return 0
            end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.samurai.useWardingCircle(player, target, {}) == xi.effect.WARDING_CIRCLE)
        assert(applied.effect == xi.effect.WARDING_CIRCLE)
        assert(applied.params.power == 17 and applied.params.duration == 210)
        assert(applied.params.origin == player)
    end)
end)
