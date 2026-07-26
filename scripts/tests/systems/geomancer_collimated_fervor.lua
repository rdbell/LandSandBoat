require('scripts/globals/job_utils/geomancer')

describe('Geomancer Collimated Fervor host', function()
    it('adds its 60-second status effect to the target with player origin', function()
        local addedEffect
        local addedOptions
        local player = {}
        local target = {
            addStatusEffect = function(_, effect, options)
                addedEffect = effect
                addedOptions = options
            end,
        }

        local result = xi.job_utils.geomancer.collimatedFervor(player, target, {})
        assert(result == xi.effect.COLLIMATED_FERVOR)
        assert(addedEffect == xi.effect.COLLIMATED_FERVOR)
        assert(addedOptions.duration == 60 and addedOptions.origin == player)
    end)
end)
