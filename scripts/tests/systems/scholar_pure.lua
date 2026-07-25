-----------------------------------
-- Pure system tests for Scholar dual-wire (slice 6749).
-- Calls production xi.job_utils.scholar pure exports.
-- Goldens match internal/scholar (5975 / 5978).
-----------------------------------

require('scripts/globals/job_utils/scholar')

local s = xi.job_utils.scholar

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Scholar pure pins', function()
    it('bases', function()
        assert(s.stratagemBasePower == 1 and s.stratagemDurationSec == 60)
        assert(s.artsDurationSec == 7200 and s.darkArtsBasePower == 1)
        assert(s.tabulaRasaDurationSec == 180 and almost(s.tabulaRasaBonusScale, 1.5))
        assert(almost(s.tabulaRasaJPMPFraction, 0.02))
        assert(s.sublimationChargeDurationSec == 7200 and s.sublimationChargeTick == 3)
        assert(s.sublimationRefreshTierBlock == 3 and s.artsBonusLevelFloor == 20)
        assert(s.enlightenmentMeritOffset == 5)
        assert(almost(s.modusVeritasBaseMultiplier, 0.5) and almost(s.modusVeritasMeritStep, 0.05))
        assert(s.modusVeritasJPPower == 3 and almost(s.modusVeritasResistFloor, 0.25))
        assert(s.oneHourRecastSecondsPerMod == 60)
        assert(s.caperEnmityPercent == 99 and almost(s.caperEnmityRange, 20.6))
        assert(s.msgEffectAlreadyActive == 523 and s.msgJAMiss == 158)
        assert(s.msgJANoEffect2 == 323 and s.msgJARecoversMP == 451)
        assert(s.msgCannotOnThatTarg == 155)
        assert(s.tabulaRasaResetRecastIDs[1] == 228)
        assert(s.tabulaRasaResetRecastIDs[2] == 231)
        assert(s.tabulaRasaResetRecastIDs[3] == 232)
        assert(not s.libraIsImplementedFromParams())
    end)
end)

describe('checks and stratagems', function()
    it('products', function()
        local msg, param = s.checkAlreadyActiveFromParams({ hasEffect = true })
        assert(msg == 523 and param == 0)
        msg, param = s.checkAlreadyActiveFromParams({ hasEffect = false })
        assert(msg == 0 and param == 0)

        msg, param = s.checkLightArtsFromParams({ hasLightArts = true, hasAddendumWhite = false })
        assert(msg == 523)
        msg, param = s.checkLightArtsFromParams({ hasLightArts = false, hasAddendumWhite = true })
        assert(msg == 523)
        msg, param = s.checkLightArtsFromParams({ hasLightArts = false, hasAddendumWhite = false })
        assert(msg == 0)

        msg, param = s.checkDarkArtsFromParams({ hasDarkArts = true, hasAddendumBlack = false })
        assert(msg == 523)
        msg, param = s.checkDarkArtsFromParams({ hasDarkArts = false, hasAddendumBlack = false })
        assert(msg == 0)

        local p = s.stratagemParamsFromParams()
        assert(p.power == 1 and p.duration == 60)
        p = s.meritStratagemParamsFromParams({ merit = 12 })
        assert(p.power == 12 and p.duration == 60)
        assert(s.enlightenmentPowerFromParams({ enlightenmentMerit = 15 }) == 10)
        p = s.enlightenmentParamsFromParams({ enlightenmentMerit = 15 })
        assert(p.power == 10 and p.duration == 60)

        msg, param = s.checkCaperEmissariusFromParams({
            hasTarget = false, isPC = true, sameAsActor = false,
        })
        assert(msg == 155)
        msg, param = s.checkCaperEmissariusFromParams({
            hasTarget = true, isPC = true, sameAsActor = true,
        })
        assert(msg == 155)
        msg, param = s.checkCaperEmissariusFromParams({
            hasTarget = true, isPC = false, sameAsActor = false,
        })
        assert(msg == 155)
        msg, param = s.checkCaperEmissariusFromParams({
            hasTarget = true, isPC = true, sameAsActor = false,
        })
        assert(msg == 0)

        local percent, range = s.caperTransferParamsFromParams()
        assert(percent == 99 and almost(range, 20.6))
    end)
end)

describe('helix regen arts tabula sublimation modus', function()
    it('products', function()
        assert(s.helixBonusFromParams({ mainJobIsSCH = false, mainLevel = 99 }) == 0)
        assert(s.helixBonusFromParams({ mainJobIsSCH = true, mainLevel = 19 }) == 0)
        assert(s.helixBonusFromParams({ mainJobIsSCH = true, mainLevel = 99 }) == 24)
        assert(s.helixBonusFromParams({ mainJobIsSCH = true, mainLevel = 80 }) == 20)

        assert(s.regenBonusFromParams({ mainJobIsSCH = false, mainLevel = 99 }) == 0)
        assert(s.regenBonusFromParams({ mainJobIsSCH = true, mainLevel = 19 }) == 0)
        assert(s.regenBonusFromParams({ mainJobIsSCH = true, mainLevel = 99 }) == 24)
        assert(s.regenBonusFromParams({ mainJobIsSCH = true, mainLevel = 50 }) == 12)
        assert(s.regenBonusFromParams({ mainJobIsSCH = true, mainLevel = 20 }) == 3)

        local plan = s.lightArtsPlanFromParams({
            lightArtsEffectMod = 7, mainJobIsSCH = true, mainLevel = 99,
        })
        assert(plan.effect == xi.effect.LIGHT_ARTS)
        assert(plan.power == 7 and plan.duration == 7200 and plan.subPower == 24)
        assert(#plan.silentDel == 1 and plan.silentDel[1] == xi.effect.DARK_ARTS)
        assert(#plan.del == 8)

        plan = s.darkArtsPlanFromParams({ mainJobIsSCH = true, mainLevel = 99 })
        assert(plan.effect == xi.effect.DARK_ARTS)
        assert(plan.power == 1 and plan.duration == 7200 and plan.subPower == 24)
        assert(#plan.silentDel == 1 and plan.silentDel[1] == xi.effect.LIGHT_ARTS)
        assert(#plan.del == 8)

        plan = s.addendumWhitePlanFromParams({
            lightArtsEffectMod = 3, mainJobIsSCH = true, mainLevel = 50,
        })
        assert(plan.effect == xi.effect.ADDENDUM_WHITE and plan.silent)
        assert(plan.power == 3 and plan.subPower == 12 and plan.duration == 7200)
        assert(#plan.silentDel == 3 and #plan.del == 0)

        plan = s.addendumBlackPlanFromParams({
            darkArtsEffectMod = 5, mainJobIsSCH = true, mainLevel = 80,
        })
        assert(plan.effect == xi.effect.ADDENDUM_BLACK and plan.silent)
        assert(plan.power == 5 and plan.subPower == 20)
        assert(#plan.silentDel == 3)

        assert(s.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 2 }) == 3480)
        assert(s.oneHourRecastFromParams({ abilityRecast = 30, oneHourRecastMod = 1 }) == 0)

        plan = s.planTabulaRasaFromParams({
            mainJobIsSCH = true, mainLevel = 99, maxMP = 1000, jpLevel = 5,
        })
        -- helix=24, regen=24 → floor(*1.5)=36; JP MP = 100
        assert(plan.power == 36 and plan.subPower == 36 and plan.duration == 180)
        assert(plan.jpMPRestore == 100)
        assert(plan.resetRecastIDs[1] == 228 and plan.resetRecastIDs[2] == 231 and plan.resetRecastIDs[3] == 232)
        assert(s.planTabulaRasaFromParams({
            mainJobIsSCH = true, mainLevel = 99, maxMP = 1000, jpLevel = 0,
        }).jpMPRestore == 0)
        plan = s.planTabulaRasaFromParams({
            mainJobIsSCH = false, mainLevel = 99, maxMP = 1000, jpLevel = 0,
        })
        assert(plan.power == 0 and plan.subPower == 0)

        -- Sublimation complete: 200 stored, 900/1000 → recover 100
        plan = s.planSublimationFromParams({
            hasComplete = true, hasActivated = false, storedPower = 200,
            currentMP = 900, maxMP = 1000, refreshTier = 0,
        })
        assert(plan.kind == s.sublimationRecoverComplete)
        assert(plan.mpRecover == 100 and plan.msgId == 451)
        assert(plan.clearEffect == xi.effect.SUBLIMATION_COMPLETE)
        assert(plan.returnMPAmount == 100)

        plan = s.planSublimationFromParams({
            hasComplete = false, hasActivated = true, storedPower = 50,
            currentMP = 100, maxMP = 1000, refreshTier = 0,
        })
        assert(plan.kind == s.sublimationRecoverActivated and plan.mpRecover == 50)

        plan = s.planSublimationFromParams({
            hasComplete = false, hasActivated = false, storedPower = 0,
            currentMP = 500, maxMP = 1000, refreshTier = 1,
        })
        assert(plan.kind == s.sublimationStartCharge and plan.delRefresh)
        assert(plan.chargeDuration == 7200 and plan.chargeTick == 3)

        plan = s.planSublimationFromParams({
            hasComplete = false, hasActivated = false, storedPower = 0,
            currentMP = 500, maxMP = 1000, refreshTier = 3,
        })
        assert(plan.kind == s.sublimationBlockedByRefresh and plan.msgId == 323)

        -- Modus Veritas
        plan = s.planModusVeritasFromParams({
            hasHelix = false, helixSubPower = 0, helixPower = 0,
            helixDurationSec = 0, remainingSec = 0, resist = 1.0, isNM = false,
            modusVeritasMerit = 0, modusVeritasJP = 0,
        })
        assert(plan.outcome == s.modusVeritasNoEffect and plan.msgId == 323)

        plan = s.planModusVeritasFromParams({
            hasHelix = true, helixSubPower = 1, helixPower = 10,
            helixDurationSec = 60, remainingSec = 30, resist = 1.0, isNM = false,
            modusVeritasMerit = 0, modusVeritasJP = 0,
        })
        assert(plan.outcome == s.modusVeritasMiss and plan.msgId == 158 and plan.returnValue == 0)

        plan = s.planModusVeritasFromParams({
            hasHelix = true, helixSubPower = 0, helixPower = 10,
            helixDurationSec = 60, remainingSec = 30, resist = 1.0, isNM = true,
            modusVeritasMerit = 0, modusVeritasJP = 0,
        })
        assert(plan.outcome == s.modusVeritasMiss)

        plan = s.planModusVeritasFromParams({
            hasHelix = true, helixSubPower = 0, helixPower = 10,
            helixDurationSec = 60, remainingSec = 30, resist = 0.2, isNM = false,
            modusVeritasMerit = 0, modusVeritasJP = 0,
        })
        assert(plan.outcome == s.modusVeritasMiss)

        -- helixPower=20, remaining=40, duration=100, merit=2, jp=3
        -- mult=0.5+0.1=0.6; newSub=1; newPower=40+9=49
        -- newDur=(100-40)+floor(40*0.6)=60+24=84; ms=84000
        plan = s.planModusVeritasFromParams({
            hasHelix = true, helixSubPower = 0, helixPower = 20,
            helixDurationSec = 100, remainingSec = 40, resist = 1.0, isNM = false,
            modusVeritasMerit = 2, modusVeritasJP = 3,
        })
        assert(plan.outcome == s.modusVeritasApply)
        assert(plan.newSubPower == 1 and plan.newHelixPower == 49)
        assert(plan.newDurationSec == 84 and plan.newDurationMs == 84000)
        assert(plan.msgId == 0)
    end)
end)
