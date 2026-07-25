-----------------------------------
-- Pure system tests for Dragoon dual-wire (slice 6748).
-- Calls production xi.job_utils.dragoon pure exports.
-- Goldens match internal/dragoon (0890 / 6032 / 6034).
-----------------------------------

require('scripts/globals/job_utils/dragoon')

local d = xi.job_utils.dragoon

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Dragoon pure pins', function()
    it('bases', function()
        assert(d.ancientCircleBaseDuration == 180)
        assert(d.ancientCircleMainPower == 15 and d.ancientCircleSubPower == 5)
        assert(d.spiritSurgeDuration == 60 and almost(d.spiritSurgeMaxHPFraction, 0.25))
        assert(d.angonBaseDuration == 15 and d.angonPower == 20)
        assert(almost(d.steadyWingMissingHPFraction, 0.3))
        assert(d.spiritLinkRegenDuration == 90 and d.spiritLinkRegenTick == 3)
        assert(d.wyvernBreathGearCap == 64 and d.wyvernExpCap == 1000)
        assert(d.highJumpEnmityMain == 50 and d.highJumpEnmitySub == 30)
        assert(d.recastJump == 158 and d.recastSpiritJump == 166)
        assert(d.oneHourRecastSecondsPerMod == 60)
    end)
end)

describe('jump multipliers and ancient circle', function()
    it('products', function()
        -- Jump TP: floor(100 * (100+0)/100) = 100
        assert(d.jumpTPBonusFromParams({
            jumpTPBonus = 100, spiritJumpBonus = 0, storeTP = 0, isSpiritJump = false,
        }) == 100)
        -- spirit jump + storeTP 50: floor((100+20)*(150)/100) = floor(180) = 180
        assert(d.jumpTPBonusFromParams({
            jumpTPBonus = 100, spiritJumpBonus = 20, storeTP = 50, isSpiritJump = true,
        }) == 180)

        assert(d.wyvernSTRBoostFromParams(0) == 1)
        assert(d.wyvernSTRBoostFromParams(25) == 6)

        assert(almost(d.jumpATTMultiplierFromParams(0), 1.0))
        assert(almost(d.jumpATTMultiplierFromParams(25), 1.25))
        assert(almost(d.jumpFTPFromVITFromParams(256), 2.0))

        local atk, tp, force = d.spiritJumpMultipliersFromParams({
            jumpATTBonus = 0, soulSpiritATTBonus = 0, hasWyvern = false,
        })
        assert(almost(atk, 1.0) and almost(tp, 1) and not force)
        atk, tp, force = d.spiritJumpMultipliersFromParams({
            jumpATTBonus = 0, soulSpiritATTBonus = 0, hasWyvern = true,
        })
        assert(almost(atk, 1.25) and almost(tp, 2) and force)

        atk, tp, force = d.soulJumpMultipliersFromParams({
            jumpATTBonus = 0, soulSpiritATTBonus = 0, hasWyvern = true,
        })
        assert(almost(atk, 1.5) and almost(tp, 3) and force)

        assert(d.ancientCircleDurationFromParams(0) == 180)
        assert(d.ancientCircleDurationFromParams(30) == 210)
        assert(d.ancientCirclePowerFromParams({
            mainJobIsDRG = false, ancientCircleJP = 5, potencyMod = 0,
        }) == 5)
        assert(d.ancientCirclePowerFromParams({
            mainJobIsDRG = true, ancientCircleJP = 5, potencyMod = 2,
        }) == 22)

        assert(almost(d.spiritSurgeMaxHPBoostFromParams(1000), 250))
        assert(d.angonDurationFromParams(15) == 30)
        assert(almost(d.steadyWingStoneskinPowerFromParams({
            maxHP = 1000, currentHP = 700,
        }), 300 + 300))
        assert(d.spiritLinkRegenPowerFromParams(75) == 25)
        assert(almost(d.spiritLinkTPShareFromParams(1000), 500))
    end)
end)

describe('breath and exp and high jump', function()
    it('products', function()
        assert(almost(d.deepBreathingBonusFromParams({
            hasEffect = false, deepBreathingMerits = 5, enhanceDB = true, isHealing = true,
        }), 0))
        assert(almost(d.deepBreathingBonusFromParams({
            hasEffect = true, deepBreathingMerits = 1, enhanceDB = false, isHealing = true,
        }), 50))
        assert(almost(d.deepBreathingBonusFromParams({
            hasEffect = true, deepBreathingMerits = 1, enhanceDB = true, isHealing = true,
        }), 55))
        assert(almost(d.deepBreathingBonusFromParams({
            hasEffect = true, deepBreathingMerits = 1, enhanceDB = false, isHealing = false,
        }), 1.0))

        assert(d.wyvernBreathJPBonusFromParams(3) == 30)
        assert(almost(d.breathAugmentsBonusFromParams(10), 1.1))
        assert(almost(d.damageBreathAugmentsFromParams(10), 0.1))

        -- healing breath HB I: base 8 mult 35, maxHP 1000, gear 0 deep 0 jp 0 aug 1
        -- mult = 35/256; floor(1000*35/256)+8 = floor(136.718)+8 = 144
        local cure = d.healingBreathCurePowerFromParams({
            wyvernMaxHP = 1000, base = 8, baseMult = 35, gear = 0,
            deepMult = 0, jobPointBonus = 0, breathAugmentsBonus = 1,
        })
        assert(almost(cure, 144))

        -- damage: floor(600/6+15+0)=115; gear 0; deep 0; aug 0 → 115
        local dmg = d.damageBreathBaseFromParams({
            wyvernHP = 600, jobPointBonus = 0, gear = 0,
            breathAugments = 0, deepMult = 0,
        })
        assert(almost(dmg, 115))

        local breath = d.breathAbilityByResRankFromParams({ 5, 3, 4, 2, 6, 1 })
        -- water has rank 1 lowest → HYDRO
        assert(breath == (xi.jobAbility and xi.jobAbility.HYDRO_BREATH or 651))
        breath = d.breathAbilityByResRankFromParams({ 2, 2, 2, 2, 2, 2 })
        assert(breath == (xi.jobAbility and xi.jobAbility.FLAME_BREATH or 646))

        local ups, newExp = d.wyvernLevelUpsFromParams({ prevExp = 0, exp = 200 })
        assert(ups == 1 and newExp == 200)
        ups, newExp = d.wyvernLevelUpsFromParams({ prevExp = 900, exp = 200 })
        -- capped add 100 → newExp 1000; floor(1000/200)-floor(900/200)=1
        assert(ups == 1 and newExp == 1000)
        ups, newExp = d.wyvernLevelUpsFromParams({ prevExp = 1000, exp = 500 })
        assert(ups == 0 and newExp == 1000)

        assert(d.breathRecastFromParams({ baseRecast = 60, dragoonBreathRecastMod = 10 }) == 50)
        assert(d.breathRecastFromParams({ baseRecast = 5, dragoonBreathRecastMod = 10 }) == 0)

        assert(d.highJumpEnmityShedFromParams({
            mainJobIsDRG = true, highJumpEnmityReduction = 5,
        }) == 55)
        assert(d.highJumpEnmityShedFromParams({
            mainJobIsDRG = false, highJumpEnmityReduction = 0,
        }) == 30)
        assert(d.spiritSurgeHighJumpTPRemoveFromParams(100) == 200)
        assert(d.superJumpInRangeFromParams(75.0))
        assert(not d.superJumpInRangeFromParams(75.1))

        assert(d.oneHourRecastFromParams({ baseRecast = 3600, oneHourMod = 15 }) == 2700)
        assert(d.healingBreathTierFromParams(80) == (xi.jobAbility and xi.jobAbility.HEALING_BREATH_IV or 639))
        assert(d.healingBreathTierFromParams(40) == (xi.jobAbility and xi.jobAbility.HEALING_BREATH_III or 642))
        assert(d.healingBreathTierFromParams(20) == (xi.jobAbility and xi.jobAbility.HEALING_BREATH_II or 641))
        assert(d.healingBreathTierFromParams(10) == (xi.jobAbility and xi.jobAbility.HEALING_BREATH or 640))
    end)
end)
