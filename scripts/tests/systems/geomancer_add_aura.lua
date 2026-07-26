require('scripts/globals/job_utils/geomancer')

describe('Geomancer addAura host', function()
    it('adds COLURE_ACTIVE with the supplied aura fields and target origin', function()
        local addedEffect
        local addedOptions
        local target = {
            addStatusEffect = function(_, effect, options)
                addedEffect = effect
                addedOptions = options
            end,
        }

        xi.job_utils.geomancer.addAura(target, 180, xi.effect.GEO_REGEN, 25, xi.auraTarget.ALLIES)
        assert(addedEffect == xi.effect.COLURE_ACTIVE)
        assert(addedOptions.duration == 180 and addedOptions.origin == target and addedOptions.tick == 3)
        assert(addedOptions.subType == xi.effect.GEO_REGEN and addedOptions.subPower == 25)
        assert(addedOptions.tier == xi.auraTarget.ALLIES and addedOptions.flag == xi.effectFlag.AURA)
    end)
end)
