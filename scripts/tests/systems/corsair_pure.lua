-----------------------------------
-- Pure system tests for Corsair dual-wire (slice 6736).
-- Calls production xi.job_utils.corsair pure exports.
-- Goldens match internal/corsair (0885).
-----------------------------------

require('scripts/globals/job_utils/corsair')

local c = xi.job_utils.corsair

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Corsair pure pins', function()
    it('bases and messages', function()
        assert(c.baseRollDuration == 300 and c.baseBustDuration == 300)
        assert(c.doubleUpChanceDuration == 45)
        assert(c.recastReductionCap == 45 and c.recastMin == 15 and c.recastMax == 300)
        assert(c.bustThreshold == 12 and c.maxDieFace == 11)
        assert(c.msgCannotPerform == 71 and c.msgRollAlreadyActive == 429)
        assert(c.msgRollMain == 420 and c.msgDoubleUpBust == 426)
        assert(c.msgNoEligibleRoll == 428 and c.msgNoEffect == 283)
        assert(c.crookedCardsPower == 20 and c.crookedCardsDuration == 60)
        assert(c.tripleShotPower == 40 and c.tripleShotDuration == 90)

        -- Catalog sample: Corsair's Roll powers
        local row = c.rollData[xi.jobAbility.CORSAIRS_ROLL]
        assert(row and row.powers[1] == 10 and row.powers[11] == 24)
        assert(row.phantomBase == 2 and row.bustPower == 6)
    end)
end)

describe('max rolls eleven roll duration recast', function()
    it('slots eleven duration recast', function()
        assert(c.maxActiveRollsFromParams({ mainJobCOR = true }) == 2)
        assert(c.maxActiveRollsFromParams({ mainJobCOR = false }) == 1)

        assert(not c.hasElevenRollFromParams({}))
        assert(not c.hasElevenRollFromParams({ 5, 7, 10 }))
        assert(c.hasElevenRollFromParams({ 5, 11, 3 }))

        assert(c.rollDurationFromParams({}) == 300)
        assert(c.rollDurationFromParams({
            winningStreakMerit = 10, phantomDurationMod = 20, phantomRollDurationJP = 5,
        }) == 300 + 10 + 20 + 10)

        assert(c.bustDurationFromParams({}) == 300)
        assert(c.bustDurationFromParams({ bustDurationMerit = 30 }) == 270)

        assert(c.phantomRecastFromParams({
            abilityRecast = 60, meritRecast = 0, modRecast = 0,
            elevenRollActive = false, bustCount = 0,
        }) == 60)
        assert(c.phantomRecastFromParams({
            abilityRecast = 60, meritRecast = 0, modRecast = 0,
            elevenRollActive = true, bustCount = 0,
        }) == 30)
        assert(c.phantomRecastFromParams({
            abilityRecast = 60, meritRecast = 0, modRecast = 0,
            elevenRollActive = true, bustCount = 1,
        }) == 60)
        -- reduction clamped to 45
        assert(c.phantomRecastFromParams({
            abilityRecast = 100, meritRecast = 30, modRecast = 30,
            elevenRollActive = false, bustCount = 0,
        }) == 55)
        -- min 15
        assert(c.phantomRecastFromParams({
            abilityRecast = 20, meritRecast = 20, modRecast = 0,
            elevenRollActive = false, bustCount = 0,
        }) == 15)
        -- max 300
        assert(c.phantomRecastFromParams({
            abilityRecast = 400, meritRecast = 0, modRecast = 0,
            elevenRollActive = false, bustCount = 0,
        }) == 300)
    end)
end)

describe('RollPower pure product', function()
    it('power job bonus enhance level', function()
        local powers = { 10, 11, 11, 12, 20, 13, 15, 16, 8, 17, 24 } -- Corsair's

        -- identity total 5
        assert(almost(c.rollPowerFromParams({
            total = 5, powers = powers, bustPower = 6, bonus = 0, bonusJob = 17,
            phantomBase = 2, jobBonusActive = false, enhanceTriggered = false,
            phantomMult = 0, actorLevel = 99, targetLevel = 99,
        }), 20))

        -- bust total 12
        assert(almost(c.rollPowerFromParams({
            total = 12, powers = powers, bustPower = 6, bonus = 0, bonusJob = 17,
            phantomBase = 2, jobBonusActive = true, enhanceTriggered = false,
            phantomMult = 0, actorLevel = 99, targetLevel = 99,
        }), 6))

        -- job bonus on non-bust
        local ninjaPowers = { 10, 13, 15, 40, 18, 20, 25, 5, 28, 30, 50 }
        assert(almost(c.rollPowerFromParams({
            total = 4, powers = ninjaPowers, bustPower = 15, bonus = 15, bonusJob = 13,
            phantomBase = 2, jobBonusActive = true, enhanceTriggered = false,
            phantomMult = 0, actorLevel = 99, targetLevel = 99,
        }), 40 + 15))

        -- phantom mult
        assert(almost(c.rollPowerFromParams({
            total = 1, powers = powers, bustPower = 6, bonus = 0, bonusJob = 17,
            phantomBase = 2, jobBonusActive = false, enhanceTriggered = false,
            phantomMult = 3, actorLevel = 99, targetLevel = 99,
        }), 10 + 2 * 3))

        -- level correction
        assert(almost(c.rollPowerFromParams({
            total = 1, powers = powers, bustPower = 6, bonus = 0, bonusJob = 17,
            phantomBase = 2, jobBonusActive = false, enhanceTriggered = false,
            phantomMult = 0, actorLevel = 50, targetLevel = 100,
        }), 10 * 50 / 100))

        -- enhance for bonusJob NONE (Bolters-like): use bonusJob 0
        local bolterPowers = { 6, 6, 12, 3, 9, 15, 18, 21, 1, 24, 30 }
        assert(almost(c.rollPowerFromParams({
            total = 3, powers = bolterPowers, bustPower = 6, bonus = 5, bonusJob = 0,
            phantomBase = 1, jobBonusActive = false, enhanceTriggered = true,
            phantomMult = 0, actorLevel = 99, targetLevel = 99,
        }), 12 + 5))

        -- invalid total
        assert(almost(c.rollPowerFromParams({
            total = 0, powers = powers, bustPower = 6,
        }), 0))
    end)
end)

describe('ability check and apply message', function()
    it('gates and messages', function()
        local msg, ok = c.onRollAbilityCheckFromParams({
            hasSameRoll = true, numBusts = 0, maxRolls = 2,
        })
        assert(not ok and msg == 429)

        msg, ok = c.onRollAbilityCheckFromParams({
            hasSameRoll = false, numBusts = 2, maxRolls = 2,
        })
        assert(not ok and msg == 71)

        msg, ok = c.onRollAbilityCheckFromParams({
            hasSameRoll = false, numBusts = 1, maxRolls = 2,
        })
        assert(ok and msg == 0)

        msg, ok = c.checkDoubleUpFromParams({ hasDoubleUpChance = false })
        assert(not ok and msg == 428)
        msg, ok = c.checkDoubleUpFromParams({ hasDoubleUpChance = true })
        assert(ok and msg == 0)

        assert(c.applyMessageFromParams({
            isCaster = true, isDoubleup = false, applied = true, total = 7,
        }) == 420)
        assert(c.applyMessageFromParams({
            isCaster = true, isDoubleup = true, applied = true, total = 7,
        }) == 424)
        assert(c.applyMessageFromParams({
            isCaster = false, isDoubleup = false, applied = true, total = 7,
        }) == 421)
        assert(c.applyMessageFromParams({
            isCaster = true, isDoubleup = false, applied = true, total = 12,
        }) == 426)
        assert(c.applyMessageFromParams({
            isCaster = false, isDoubleup = false, applied = true, total = 12,
        }) == 427)
        assert(c.applyMessageFromParams({
            isCaster = true, isDoubleup = false, applied = false, total = 7,
        }) == 422)
        assert(c.applyMessageFromParams({
            isCaster = false, isDoubleup = false, applied = false, total = 7,
        }) == 283)

        local cc = c.crookedCardsFromParams()
        assert(cc.power == 20 and cc.duration == 60)
        local ts = c.tripleShotFromParams()
        assert(ts.power == 40 and ts.duration == 90)
    end)
end)
