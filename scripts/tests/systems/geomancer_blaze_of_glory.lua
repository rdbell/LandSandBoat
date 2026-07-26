require('scripts/globals/job_utils/geomancer')

describe('Geomancer Blaze of Glory host', function()
    it('adds its self-origin status effect with a three-second tick', function()
        local addedEffect
        local addedOptions
        local player = {
            addStatusEffect = function(_, effect, options)
                addedEffect = effect
                addedOptions = options
            end,
        }

        local result = xi.job_utils.geomancer.blazeOfGlory(player, {}, {})
        assert(result == xi.effect.BLAZE_OF_GLORY)
        assert(addedEffect == xi.effect.BLAZE_OF_GLORY)
        assert(addedOptions.duration == 60 and addedOptions.origin == player and addedOptions.tick == 3)
    end)
end)
