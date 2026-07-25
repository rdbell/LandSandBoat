-----------------------------------
-- Pure system tests for Ninja ability dual-wire (slice 6750).
-- Calls production xi.job_utils.ninja pure exports.
-- Goldens match internal/ninja (0899).
-----------------------------------

require('scripts/globals/job_utils/ninja')

local n = xi.job_utils.ninja

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Ninja pure pins', function()
    it('bases', function()
        assert(n.oneHourRecastSecondsPerMod == 60)
        assert(n.yoninPower == 30 and n.yoninDurationSec == 300 and n.yoninTickSec == 15)
        assert(n.inninPower == 30 and n.inninDurationSec == 300 and n.inninTickSec == 15)
        assert(n.inninSubPower == 20)
        assert(n.sangeDurationSec == 60 and n.sangeMeritOffset == 1 and n.sangeMeritPowerScale == 25)
        assert(n.futaeDurationSec == 60)
        assert(n.issekiganPower == 25 and n.issekiganDurationSec == 60)
        assert(n.mikageDurationSec == 45)
        assert(almost(n.mijinGakureHPFraction, 0.8) and almost(n.mijinGakureJPScale, 0.03))
    end)
end)

describe('oneHour recast and fixed params', function()
    it('products', function()
        assert(n.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(n.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(n.oneHourRecastFromParams({ abilityRecast = 60, oneHourRecastMod = 5 }) == 0)
        assert(n.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)

        local p = n.yoninFromParams()
        assert(p.power == 30 and p.duration == 300 and p.tick == 15)
        p = n.inninFromParams()
        assert(p.power == 30 and p.duration == 300 and p.tick == 15 and p.subPower == 20)
        p = n.futaeFromParams()
        assert(p.duration == 60)
        p = n.issekiganFromParams()
        assert(p.power == 25 and p.duration == 60)
        p = n.mikageFromParams()
        assert(p.duration == 45)

        assert(n.sangePowerFromParams({ merit = 1 }) == 0)
        assert(n.sangePowerFromParams({ merit = 2 }) == 25)
        assert(n.sangePowerFromParams({ merit = 5 }) == 100)
        assert(n.sangePowerFromParams({ merit = 0 }) == -25)
        p = n.sangeFromParams({ merit = 5 })
        assert(p.power == 100 and p.duration == 60)
    end)
end)

describe('mijin gakure damage', function()
    it('products', function()
        assert(n.mijinGakureBaseDamageFromParams({ playerHP = 1000 }) == 800)
        assert(n.mijinGakureBaseDamageFromParams({ playerHP = 0 }) == 0)
        assert(n.mijinGakureBaseDamageFromParams({ playerHP = 999 }) == 799)
        assert(n.mijinGakureBaseDamageFromParams({ playerHP = 1 }) == 0)

        assert(n.mijinGakureDamageFromParams({
            base = 800, resist = 1, tmdaFactor = 1, jpLevel = 0,
        }) == 800)
        assert(n.mijinGakureDamageFromParams({
            base = 800, resist = 0.5, tmdaFactor = 1, jpLevel = 0,
        }) == 400)
        assert(n.mijinGakureDamageFromParams({
            base = 100, resist = 0.5, tmdaFactor = 0.9, jpLevel = 0,
        }) == 45)
        assert(n.mijinGakureDamageFromParams({
            base = 800, resist = 1, tmdaFactor = 1, jpLevel = 10,
        }) == 1040)
        assert(n.mijinGakureDamageFromParams({
            base = 1000, resist = 0.5, tmdaFactor = 0.5, jpLevel = 0,
        }) == 250)
        assert(n.mijinGakureDamageFromParams({
            base = 1000, resist = 1, tmdaFactor = 1, jpLevel = 1,
        }) == 1030)
        assert(n.mijinGakureDamageFromParams({
            base = 800, resist = 0, tmdaFactor = 1, jpLevel = 5,
        }) == 0)
    end)
end)
