-----------------------------------
-- Pure system tests for Paladin dual-wire (slice 6739).
-- Calls production xi.job_utils.paladin pure exports.
-- Goldens match internal/paladin (where formulas align).
-----------------------------------

require('scripts/globals/job_utils/paladin')

local p = xi.job_utils.paladin

local function almost(a, b)
    return math.abs(a - b) < 1e-6
end

describe('Paladin pure pins', function()
    it('bases', function()
        assert(p.coverBaseDuration == 15 and p.coverBonusTimeCap == 15)
        assert(p.divineEmblemBasePower == 50 and p.divineEmblemDuration == 60)
        assert(p.fealtyBaseDuration == 60 and p.fealtyMeritUnit == 5)
        assert(p.holyCircleMainPower == 15 and p.holyCircleSubPower == 5)
        assert(p.holyCircleBaseDuration == 180)
        assert(almost(p.interveneLvlScale, 3.36))
        assert(p.palisadeBasePower == 30 and p.rampartBaseDuration == 30)
        assert(p.sentinelBasePct == 90 and p.sepulcherBaseDuration == 180)
        assert(almost(p.shieldBashLvlScale, 0.273))
        assert(almost(p.chivalryBaseMP, 0.05) and almost(p.chivalryMNDScale, 0.0015))
        assert(p.oneHourRecastSecondsPerMod == 60)
    end)
end)

describe('Cover Chivalry HolyCircle products', function()
    it('formulas', function()
        assert(p.coverBonusTimeFromParams({ playerVIT = 100, playerMND = 100, targetVIT = 50 }) == 15)
        assert(p.coverBonusTimeFromParams({ playerVIT = 10, playerMND = 10, targetVIT = 100 }) == 0)
        assert(p.coverBonusTimeFromParams({ playerVIT = 40, playerMND = 40, targetVIT = 20 }) == 10)

        assert(p.coverDurationFromParams({
            bonusTime = 10, merit = 5, coverDurationMod = 2, jp = 3,
        }) == 15 + 10 + 5 + 2 + 3)

        assert(p.divineEmblemPowerFromParams(0) == 50)
        assert(p.divineEmblemPowerFromParams(20) == 70)

        -- merits 5 → duration 60; merits 10 → 60+(10-5)+(2)*enh
        assert(p.fealtyDurationFromParams({ merits = 5, enhancesFealty = 0 }) == 60)
        assert(p.fealtyDurationFromParams({ merits = 10, enhancesFealty = 5 }) == 60 + 5 + 10)

        assert(p.holyCirclePowerFromParams({ isMain = true, potency = 5 }) == 20)
        assert(p.holyCirclePowerFromParams({ isMain = false, potency = 5 }) == 10)
        assert(p.holyCircleDurationFromParams(20) == 200)

        -- Chivalry: tp=1000, mnd=100, enhances=0, merits=5 → ~200
        assert(almost(p.chivalryMPFromParams({
            tp = 1000, mnd = 100, enhancesChivalry = 0, merits = 5,
        }), 200))
    end)
end)

describe('Intervene Sentinel recast damage', function()
    it('products', function()
        assert(p.interveneDamageFromParams(75) == 252)
        assert(p.shieldBashBaseDamageFromParams(75) == 20)
        assert(p.palisadePowerFromParams(5) == 35)
        assert(p.rampartDurationFromParams(10) == 40)
        assert(p.sentinelPowerFromParams(10) == 10000)
        assert(p.sentinelDurationFromParams(5) == 35)
        assert(p.sepulcherDurationFromParams(10) == 190)

        assert(p.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 10 }) == 3000)
        assert(p.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 10 }) == 0)
    end)
end)
