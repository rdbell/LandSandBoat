-----------------------------------
-- Pure system tests for Dancer dual-wire (slice 6735).
-- Calls production xi.job_utils.dancer pure exports.
-- Goldens match internal/dancer (0881).
-----------------------------------

require('scripts/globals/job_utils/dancer')

local d = xi.job_utils.dancer

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Dancer pure pins', function()
    it('bases and catalogs', function()
        assert(d.baseMaxFinishingMoves == 5)
        assert(d.finishingMoveDurationSec == 7200)
        assert(d.reverseFlourishMoveCap == 5)
        assert(d.buildingFlourishMaxPower == 3)
        assert(d.waltzPotencyCap == 50)
        assert(d.waltzCostPerMod == 10)
        assert(d.fanDanceMeritUnit == 5)
        assert(d.defaultCurePower == 1.0)
        assert(d.stepPrestoAward == 5 and d.stepMainDNCAward == 2 and d.stepBaseAward == 1)
        assert(d.animatedFlourishVEHigh == 1500 and d.animatedFlourishVELow == 1000)
        assert(d.msgNotEnoughTP == 192 and d.msgNoFinishingMoves == 524)
        assert(d.msgRequiresCombat == 525 and d.msgNoFootRiseFull == 561)

        local w = d.lookupWaltz(190)
        assert(w and w[1] == 200 and almost(w[2], 0.25) and w[3] == 60)
        w = d.lookupWaltz(311)
        assert(w and w[1] == 800 and almost(w[2], 1.25) and w[3] == 600)
        w = d.lookupWaltz(262)
        assert(w and w[1] == 800 and almost(w[2], 0.75) and w[3] == 270)
        assert(d.lookupWaltz(194) == nil)

        assert(d.isDivineWaltz(195) and d.isDivineWaltz(262))
        assert(not d.isDivineWaltz(190))
    end)
end)

describe('Waltz pure cost amount recast', function()
    it('cost amount recast', function()
        assert(d.waltzCostFromParams({ baseCost = 200, waltzCostMod = 0 }) == 200)
        assert(d.waltzCostFromParams({ baseCost = 200, waltzCostMod = 5 }) == 150)

        local cost, ok = d.waltzCostForAbilityFromParams({ abilityId = 190, waltzCostMod = 2 })
        assert(ok and cost == 180)
        cost, ok = d.waltzCostForAbilityFromParams({ abilityId = 999 })
        assert(not ok and cost == 0)

        local amt
        amt, ok = d.waltzAmountFromParams({
            abilityId = 190, mainJobDNC = true,
            targetVIT = 100, casterCHR = 100,
            waltzPotency = 0, hasContradance = false,
            curePower = 1.0, targetMissingHP = 9999,
        })
        -- (100+100)*0.25 + 60 = 110
        assert(ok and almost(amt, 110))

        amt, ok = d.waltzAmountFromParams({
            abilityId = 190, mainJobDNC = false,
            targetVIT = 100, casterCHR = 100,
            waltzPotency = 0, hasContradance = false,
            curePower = 1.0, targetMissingHP = 9999,
        })
        -- mult halved: (200)*0.125 + 60 = 85
        assert(ok and almost(amt, 85))

        amt, ok = d.waltzAmountFromParams({
            abilityId = 190, mainJobDNC = true,
            targetVIT = 100, casterCHR = 100,
            waltzPotency = 50, hasContradance = true,
            curePower = 1.0, targetMissingHP = 9999,
        })
        -- floor(110 * 1.5) * 2 = 165 * 2 = 330
        assert(ok and almost(amt, 330))

        amt, ok = d.waltzAmountFromParams({
            abilityId = 190, mainJobDNC = true,
            targetVIT = 100, casterCHR = 100,
            waltzPotency = 0, hasContradance = false,
            curePower = 1.0, targetMissingHP = 50,
        })
        assert(ok and almost(amt, 50))

        -- potency cap 50
        amt, ok = d.waltzAmountFromParams({
            abilityId = 190, mainJobDNC = true,
            targetVIT = 0, casterCHR = 0,
            waltzPotency = 99, hasContradance = false,
            curePower = 1.0, targetMissingHP = 9999,
        })
        -- floor(60 * 1.5) = 90
        assert(ok and almost(amt, 90))

        assert(almost(d.waltzRecastFromParams({
            baseRecast = 20, waltzDelay = 0, hasFanDance = false, fanDanceMeritValue = 0,
        }), 20))
        assert(almost(d.waltzRecastFromParams({
            baseRecast = 20, waltzDelay = -2, hasFanDance = false, fanDanceMeritValue = 0,
        }), 18))
        -- Fan Dance merit 10 (>5): * (105-10)/100 = *0.95
        assert(almost(d.waltzRecastFromParams({
            baseRecast = 20, waltzDelay = 0, hasFanDance = true, fanDanceMeritValue = 10,
        }), 19))
        -- merit == 5 does not apply
        assert(almost(d.waltzRecastFromParams({
            baseRecast = 20, waltzDelay = 0, hasFanDance = true, fanDanceMeritValue = 5,
        }), 20))
        assert(almost(d.waltzRecastFromParams({
            baseRecast = 5, waltzDelay = -10, hasFanDance = false, fanDanceMeritValue = 0,
        }), 0))
    end)
end)

describe('Finishing moves reverse animated building', function()
    it('FM products', function()
        assert(d.maxFinishingMovesFromParams({ bonus = 0 }) == 5)
        assert(d.maxFinishingMovesFromParams({ bonus = 2 }) == 7)

        assert(d.stepFinishingMovesBaseFromParams({}) == 1)
        assert(d.stepFinishingMovesBaseFromParams({ mainJobDNC = true }) == 2)
        assert(d.stepFinishingMovesBaseFromParams({ hasPresto = true, mainJobDNC = true }) == 5)
        assert(d.stepFinishingMovesBaseFromParams({
            mainJobDNC = true, mainHandIsTerps = true, stepFinishMod = 3,
        }) == 5)

        assert(d.finishingMoveIconFromParams(0) == nil)
        assert(d.finishingMoveIconFromParams(1) == 381)
        assert(d.finishingMoveIconFromParams(5) == 385)
        assert(d.finishingMoveIconFromParams(6) == 588)
        assert(d.finishingMoveIconFromParams(10) == 588)

        assert(d.clampFinishingMovesFromParams({ numMoves = 3, max = 5 }) == 3)
        assert(d.clampFinishingMovesFromParams({ numMoves = 9, max = 5 }) == 5)
        assert(d.clampFinishingMovesFromParams({ numMoves = -1, max = 5 }) == 0)

        local tp, used = d.reverseFlourishTPFromParams({
            numMoves = 3, flourishIIJP = 0, reverseFlourishGear = 0, reverseFlourishMerits = 0,
        })
        -- (95)*3 + 5*9 = 285 + 45 = 330
        assert(tp == 330 and used == 3)

        tp, used = d.reverseFlourishTPFromParams({
            numMoves = 8, flourishIIJP = 5, reverseFlourishGear = 2, reverseFlourishMerits = 1,
        })
        -- used=5: (100)*5 + (7)*25 + 30 = 500 + 175 + 30 = 705
        assert(tp == 705 and used == 5)

        assert(d.buildingFlourishPowerFromParams(2) == 2)
        assert(d.buildingFlourishPowerFromParams(5) == 3)
        assert(d.buildingFlourishPowerFromParams(-1) == 0)

        local plan = d.animatedFlourishFromParams(1)
        assert(plan.ve == 1000 and plan.usedMoves == 1)
        plan = d.animatedFlourishFromParams(2)
        assert(plan.ve == 1500 and plan.usedMoves == 2)
        plan = d.animatedFlourishFromParams(5)
        assert(plan.ve == 1500 and plan.usedMoves == 2)
    end)
end)

describe('animations actionInfo terpsichore', function()
    it('tables', function()
        assert(d.stepAnimationFromParams(0) == 15)
        assert(d.stepAnimationFromParams(3) == 14) -- sword
        assert(d.flourishAnimationFromParams(3) == 24)
        assert(d.stepAnimationFromParams(12) == 23)
        assert(d.flourishAnimationFromParams(12) == 33)
        assert(d.stepAnimationFromParams(13) == 0)
        assert(d.flourishAnimationFromParams(99) == 0)

        local miss, hit, ok = d.lookupActionInfoFromParams(201)
        assert(ok and miss == 1 and hit == 5)
        miss, hit, ok = d.lookupActionInfoFromParams(312)
        assert(ok and miss == 4 and hit == 8)
        miss, hit, ok = d.lookupActionInfoFromParams(999)
        assert(not ok)

        assert(d.isTerpsichore(18989))
        assert(d.isTerpsichore(20584))
        assert(not d.isTerpsichore(1))
    end)
end)

describe('ability check pure gates', function()
    it('step flourish noFootRise', function()
        local msg, ok = d.checkStepAbilityFromParams({
            inCombat = false, hasTrance = false, playerTP = 1000,
        })
        assert(not ok and msg == 525)

        msg, ok = d.checkStepAbilityFromParams({
            inCombat = true, hasTrance = true, playerTP = 0,
        })
        assert(ok and msg == 0)

        msg, ok = d.checkStepAbilityFromParams({
            inCombat = true, hasTrance = false, playerTP = 99,
        })
        assert(not ok and msg == 192)

        msg, ok = d.checkStepAbilityFromParams({
            inCombat = true, hasTrance = false, playerTP = 100,
        })
        assert(ok and msg == 0)

        msg, ok = d.checkFlourishAbilityFromParams({
            combatOnly = true, inCombat = false, numFinishingMoves = 5, minimumCost = 1,
        })
        assert(not ok and msg == 525)

        msg, ok = d.checkFlourishAbilityFromParams({
            combatOnly = true, inCombat = true, numFinishingMoves = 1, minimumCost = 2,
        })
        assert(not ok and msg == 524)

        msg, ok = d.checkFlourishAbilityFromParams({
            combatOnly = false, inCombat = false, numFinishingMoves = 2, minimumCost = 2,
        })
        assert(ok and msg == 0)

        msg, ok = d.checkNoFootRiseAbilityFromParams({
            hasFMEffect = true, currentFM = 5, maxFM = 5,
        })
        assert(not ok and msg == 561)

        msg, ok = d.checkNoFootRiseAbilityFromParams({
            hasFMEffect = true, currentFM = 4, maxFM = 5,
        })
        assert(ok and msg == 0)

        msg, ok = d.checkNoFootRiseAbilityFromParams({
            hasFMEffect = false, currentFM = 0, maxFM = 5,
        })
        assert(ok and msg == 0)
    end)
end)
