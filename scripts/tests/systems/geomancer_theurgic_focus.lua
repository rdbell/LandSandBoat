require('scripts/globals/job_utils/geomancer')

describe('Geomancer Theurgic Focus host', function()
    it('adds its self-origin power-one status effect', function()
        local addedEffect
        local addedOptions
        local player = {
            addStatusEffect = function(_, effect, options)
                addedEffect = effect
                addedOptions = options
            end,
        }

        local result = xi.job_utils.geomancer.theurgicFocus(player, {}, {})
        assert(result == xi.effect.THEURGIC_FOCUS)
        assert(addedEffect == xi.effect.THEURGIC_FOCUS)
        assert(addedOptions.power == 1 and addedOptions.duration == 60 and addedOptions.origin == player)
    end)
end)
