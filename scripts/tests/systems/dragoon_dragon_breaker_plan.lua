require('scripts/globals/job_utils/dragoon')

describe('Dragoon Dragon Breaker plan', function()
    it('applies Dragon Breaker to the target', function()
        local player = {}
        local target = {
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.DRAGON_BREAKER)
                assert(params.power == 20)
                assert(params.duration == 180)
                assert(params.origin == player)
            end,
        }

        xi.job_utils.dragoon.useDragonBreaker(player, target, {})
    end)
end)
