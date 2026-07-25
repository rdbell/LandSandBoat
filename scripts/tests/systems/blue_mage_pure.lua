-----------------------------------
-- Pure system tests for Blue Mage dual-wire (slice 6742).
-- Calls production xi.job_utils.blue_mage pure exports.
-- Goldens match internal/bluemage (0900).
-----------------------------------

require('scripts/globals/job_utils/blue_mage')

local b = xi.job_utils.blue_mage

describe('Blue Mage pure pins', function()
    it('bases and fixed params', function()
        assert(b.azureLorePower == 1 and b.azureLoreDuration == 30)
        assert(b.burstAffinityPower == 1 and b.burstAffinityDuration == 30)
        assert(b.chainAffinityPower == 1 and b.chainAffinityDuration == 30)
        assert(b.diffusionPower == 1 and b.diffusionDuration == 60)
        assert(b.convergencePower == 1 and b.convergenceDuration == 60)
        assert(b.effluxPower == 16 and b.effluxDuration == 60 and b.effluxTick == 1)
        assert(b.unbridledWisdomPower == 16 and b.unbridledWisdomDuration == 30 and b.unbridledWisdomTick == 1)
        assert(b.unbridledLearningPower == 16 and b.unbridledLearningDuration == 60 and b.unbridledLearningTick == 1)
        assert(b.oneHourRecastSecondsPerMod == 60)
        assert(b.msgEffectAlreadyActive == 523)

        local p = b.azureLoreFromParams()
        assert(p.power == 1 and p.duration == 30)
        p = b.effluxFromParams()
        assert(p.power == 16 and p.duration == 60 and p.tick == 1)
        p = b.unbridledWisdomFromParams()
        assert(p.power == 16 and p.duration == 30 and p.tick == 1)
        p = b.unbridledLearningFromParams()
        assert(p.power == 16 and p.duration == 60 and p.tick == 1)
        p = b.diffusionFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = b.convergenceFromParams()
        assert(p.power == 1 and p.duration == 60)
    end)
end)

describe('oneHour recast and already-active checks', function()
    it('recast diffusion convergence', function()
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(b.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)
        assert(b.oneHourRecastFromParams({ abilityRecast = 0, oneHourRecastMod = 1 }) == 0)

        local msg, ok = b.checkDiffusionFromParams({ hasEffect = false })
        assert(ok and msg == 0)
        msg, ok = b.checkDiffusionFromParams({ hasEffect = true })
        assert(not ok and msg == 523)

        msg, ok = b.checkConvergenceFromParams({ hasEffect = false })
        assert(ok and msg == 0)
        msg, ok = b.checkConvergenceFromParams({ hasEffect = true })
        assert(not ok and msg == 523)
    end)
end)
