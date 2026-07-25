-----------------------------------
-- Pure system tests for Black Mage dual-wire (slice 6738).
-- Calls production xi.job_utils.black_mage pure exports.
-- Goldens match internal/blackmage (0896).
-----------------------------------

require('scripts/globals/job_utils/black_mage')

local b = xi.job_utils.black_mage

describe('Black Mage pure pins', function()
    it('bases and fixed params', function()
        assert(b.cascadePower == 1 and b.cascadeDuration == 60)
        assert(b.elementalSealPower == 1 and b.elementalSealDuration == 60)
        assert(b.manafontPower == 1 and b.manafontDuration == 60)
        assert(b.manaWallPower == 1 and b.manaWallDuration == 300)
        assert(b.manawellPower == 1 and b.manawellDuration == 60)
        assert(b.subtleSorceryPower == 1 and b.subtleSorceryDuration == 60)
        assert(b.enmityDouseCE == 1 and b.enmityDouseVE == 0)
        assert(b.oneHourRecastSecondsPerMod == 60)

        local p = b.cascadeFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = b.manaWallFromParams()
        assert(p.power == 1 and p.duration == 300)
        p = b.subtleSorceryFromParams()
        assert(p.power == 1 and p.duration == 60)
    end)
end)

describe('oneHour recast and enmity douse', function()
    it('recast douse', function()
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(b.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)

        local applies, ce, ve = b.enmityDouseFromParams({ isMob = true })
        assert(applies and ce == 1 and ve == 0)
        applies, ce, ve = b.enmityDouseFromParams({ isMob = false })
        assert(not applies)
    end)
end)
