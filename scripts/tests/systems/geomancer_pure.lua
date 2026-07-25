-----------------------------------
-- Pure system tests for Geomancer dual-wire (slice 6740).
-- Calls production xi.job_utils.geomancer pure exports.
-- Goldens match internal/geomancer (0894 / 5982).
-----------------------------------

require('scripts/globals/job_utils/geomancer')

local g = xi.job_utils.geomancer

local function almost(a, b)
    return math.abs(a - b) < 1e-6
end

describe('Geomancer pure pins', function()
    it('bases and catalog', function()
        assert(g.combinedSkillCap == 900)
        assert(g.indiBaseDuration == 180 and g.bolsterBaseDuration == 240)
        assert(g.widenedCompassAuraSize == 625)
        assert(g.lifeCycleMinHP == 2)
        assert(almost(g.eclipticAttritionMult, 1.25))
        assert(g.msgUnableToUseJA == 87 and g.msgRequireLuopan == 662)
        assert(g.entrustPower == 1 and g.entrustDuration == 60)

        local regen = g.potencyData[xi.effect.GEO_REGEN]
        assert(regen and regen.maxSkill == 600 and almost(regen.minPotency, 1.0) and almost(regen.maxPotency, 30.0))
        local haste = g.potencyData[xi.effect.GEO_HASTE]
        assert(haste and almost(haste.minPotency, 2.4) and almost(haste.maxPotency, 29.9))
    end)
end)

describe('EffectPotency and multipliers', function()
    it('ramp bolster bog', function()
        local pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_REGEN, combinedSkill = 0, geomancyMod = 0, hasEntrust = false,
        })
        assert(ok and almost(pot, 1.0))

        pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_REGEN, combinedSkill = 600, geomancyMod = 0, hasEntrust = false,
        })
        assert(ok and almost(pot, 30.0))

        pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_REGEN, combinedSkill = 300, geomancyMod = 0, hasEntrust = false,
        })
        assert(ok and almost(pot, 15.5))

        pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_REGEN, combinedSkill = 0, geomancyMod = 2, hasEntrust = false,
        })
        assert(ok and almost(pot, 5.0))

        pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_REGEN, combinedSkill = 0, geomancyMod = 2, hasEntrust = true,
        })
        assert(ok and almost(pot, 1.0))

        pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_HASTE, combinedSkill = 0, geomancyMod = 0, hasEntrust = false,
        })
        assert(ok and almost(pot, 240))

        pot, ok = g.effectPotencyFromParams({
            effectId = xi.effect.GEO_SLOW, combinedSkill = 0, geomancyMod = 0, hasEntrust = false,
        })
        assert(ok and almost(pot, 90))

        assert(g.combinedSkillLevelFromParams({ handbellSkill = 500, geoSkill = 500 }) == 900)
        assert(g.combinedSkillLevelFromParams({ handbellSkill = 100, geoSkill = 200 }) == 300)

        assert(almost(g.bolsteredPotency(10), 20))
        assert(almost(g.blazeOfGloryPotency(10), 15))
        assert(almost(g.finalGeoPotencyFromParams({
            base = 10, hasBolster = true, hasBlazeOfGlory = true,
        }), 20))
        assert(almost(g.finalGeoPotencyFromParams({
            base = 10, hasBolster = false, hasBlazeOfGlory = true,
        }), 15))
    end)
end)

describe('durations fullCircle lifeCycle regen', function()
    it('products', function()
        assert(g.indiDurationFromParams(0) == 180)
        assert(g.indiDurationFromParams(30) == 210)
        assert(g.bolsterDurationFromParams(0) == 240)
        assert(g.bolsterDurationFromParams(20) == 260)

        assert(g.fullCircleMPFromParams({
            mpCost = 100, hppRemaining = 50, fcMerit = 0, fcMod = 0,
        }) == 25)
        assert(g.fullCircleMPFromParams({
            mpCost = 100, hppRemaining = 50, fcMerit = 5, fcMod = 0,
        }) == 50)
        assert(g.fullCircleHPFromParams({
            mpCost = 100, hppRemaining = 50, crMerit = 1, crMod = 0,
        }) == 60)

        assert(g.lifeCycleDrainFromParams(1000) == 250)
        assert(g.lifeCycleTransferFromParams({ playerHP = 1000, lifeCycleEffectMod = 0 }) == 250)
        assert(g.lifeCycleTransferFromParams({ playerHP = 1000, lifeCycleEffectMod = 15 }) == 375)

        assert(g.lastingEmanationRegenDownFromParams({
            currentRegenDown = 20, luopanMainLvl = 99,
        }) == 13)
        assert(g.eclipticAttritionRegenDownFromParams({
            currentRegenDown = 20, luopanMainLvl = 99,
        }) == 26)
        assert(g.eclipticAttritionPotencyFromParams(40) == 50)
        assert(g.eclipticAttritionPotencyFromParams(41) == 51)
        assert(g.luopanRegenDownFromParams({ luopanMainLvl = 99, bolsterJP = 5 }) == 19)

        assert(almost(g.blazeOfGloryLuopanHPFromParams({ maxHP = 1000, blazeOfGloryJP = 10 }), 600))
        assert(g.auraSizeModFromParams(true) == 625)
        assert(g.auraSizeModFromParams(false) == 0)
    end)
end)

describe('ability check and luopan restores', function()
    it('gates mending radial entrust', function()
        local msg, ok = g.geoOnAbilityCheckFromParams({
            hasLuopan = true, isLifeCycle = false, playerHP = 1,
        })
        assert(ok and msg == 0)

        msg, ok = g.geoOnAbilityCheckFromParams({
            hasLuopan = false, isLifeCycle = true, playerHP = 2,
        })
        assert(not ok and msg == 87)

        msg, ok = g.geoOnAbilityCheckFromParams({
            hasLuopan = false, isLifeCycle = false, playerHP = 100,
        })
        assert(not ok and msg == 662)

        assert(g.mendingHalationRestoreFromParams({
            petMainLvl = 10, merit = 0, gearMod = 0, targetMaxHP = 9999, targetIsPet = false,
        }) == 70)
        assert(g.mendingHalationRestoreFromParams({
            petMainLvl = 10, merit = 0, gearMod = 0, targetMaxHP = 9999, targetIsPet = true,
        }) == 0)

        assert(g.radialArcanaRestoreFromParams({
            petMainLvl = 10, merit = 0, gearMod = 0, targetMaxMP = 9999, targetIsPet = false,
        }) == 30)

        local e = g.entrustFromParams()
        assert(e.power == 1 and e.duration == 60)
    end)
end)
