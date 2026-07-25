-----------------------------------
-- Pure system tests for Red Mage dual-wire (slice 6743).
-- Calls production xi.job_utils.red_mage pure exports.
-- Goldens match internal/redmage (0897).
-----------------------------------

require('scripts/globals/job_utils/red_mage')

local r = xi.job_utils.red_mage

describe('Red Mage pure pins', function()
    it('bases and fixed params', function()
        assert(r.chainspellPower == 1 and r.chainspellDuration == 60)
        assert(r.composurePower == 1 and r.composureDuration == 7200)
        assert(r.saboteurPower == 1 and r.saboteurDuration == 60)
        assert(r.spontaneityPower == 1 and r.spontaneityDuration == 60)
        assert(r.stymiePower == 1 and r.stymieDuration == 60)
        assert(r.oneHourRecastSecondsPerMod == 60)

        local p = r.chainspellFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = r.composureFromParams()
        assert(p.power == 1 and p.duration == 7200)
        p = r.saboteurFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = r.spontaneityFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = r.stymieFromParams()
        assert(p.power == 1 and p.duration == 60)
    end)
end)

describe('oneHour recast and Convert swap', function()
    it('recast convert products', function()
        assert(r.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(r.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(r.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)

        assert(r.convertJPExtraHPFromParams({ playerMaxHP = 1000, convertJP = 0 }) == 0)
        assert(r.convertJPExtraHPFromParams({ playerMaxHP = 1000, convertJP = 10 }) == 100)
        assert(r.convertJPExtraHPFromParams({ playerMaxHP = 999, convertJP = 10 }) == 99)

        assert(r.convertMurgleisExtraHPFromParams({ playerMaxHP = 1000, augmentsConvert = 0 }) == 0)
        assert(r.convertMurgleisExtraHPFromParams({ playerMaxHP = 1000, augmentsConvert = -5 }) == 0)
        assert(r.convertMurgleisExtraHPFromParams({ playerMaxHP = 1000, augmentsConvert = 25 }) == 250)
        assert(r.convertMurgleisExtraHPFromParams({ playerMaxHP = 999, augmentsConvert = 1 }) == 9)

        assert(not r.canConvertFromParams(0))
        assert(not r.canConvertFromParams(-1))
        assert(r.canConvertFromParams(1))

        local newHP, newMP, ok = r.convertSwapFromParams({
            playerHP = 800, playerMP = 0, playerMaxHP = 1000, convertJP = 10, augmentsConvert = 25,
        })
        assert(not ok and newHP == 0 and newMP == 0)

        newHP, newMP, ok = r.convertSwapFromParams({
            playerHP = 800, playerMP = 200, playerMaxHP = 1000, convertJP = 0, augmentsConvert = 0,
        })
        assert(ok and newHP == 200 and newMP == 800)

        newHP, newMP, ok = r.convertSwapFromParams({
            playerHP = 800, playerMP = 200, playerMaxHP = 1000, convertJP = 10, augmentsConvert = 25,
        })
        assert(ok and newHP == 550 and newMP == 800)

        newHP, newMP, ok = r.convertSwapFromParams({
            playerHP = 500, playerMP = 100, playerMaxHP = 999, convertJP = 10, augmentsConvert = 1,
        })
        assert(ok and newHP == 208 and newMP == 500)
    end)
end)
