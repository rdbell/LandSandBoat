-----------------------------------
-- Pure system tests for White Mage dual-wire (slice 6731).
-- Calls production xi.job_utils.white_mage pure exports.
-- Goldens match internal/whitemage (0895).
-----------------------------------

require('scripts/globals/job_utils/white_mage')

local w = xi.job_utils.white_mage

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('White Mage pure pins', function()
    it('bases and catalog size', function()
        assert(w.afflatusPower == 8 and w.afflatusDuration == 7200)
        assert(w.asylumPower == 3 and w.asylumDuration == 30)
        assert(w.divineSealPower == 1 and w.divineSealDuration == 60)
        assert(w.divineCaressPower == 3 and w.divineCaressDuration == 60)
        assert(w.sacrosanctityPower == 3 and w.sacrosanctityDuration == 60)
        assert(w.benedictionDoomChance == 33)
        assert(w.devotionMeritBase == 5 and w.devotionBaseMPPercent == 25)
        assert(w.martyrMeritBase == 5 and w.martyrBaseHPPercent == 200)
        assert(w.minHPForDevotionMartyr == 4)
        assert(w.msgCannotPerformTarg == 72 and w.msgUnableToUseJA == 87)
        assert(w.removablesCount() == 41)
    end)
end)

describe('isRemovable / removables catalog', function()
    it('first last mid and doom exclusion', function()
        assert(w.removables[1] == xi.effect.FLASH)
        assert(w.removables[2] == xi.effect.BLINDNESS)
        assert(w.removables[41] == xi.effect.PETRIFICATION)
        assert(w.isRemovable(xi.effect.POISON))
        assert(w.isRemovable(xi.effect.HELIX))
        assert(not w.isRemovable(xi.effect.DOOM))
        assert(not w.isRemovable(0))
        assert(not w.isRemovable(9999))
    end)
end)

describe('benedictionHeal products', function()
    it('level ratio clamp and doom chance', function()
        assert(w.benedictionHealFromParams({
            targetMaxHP = 1000, casterMainLvl = 75, targetMainLvl = 75,
        }) == 1000)
        assert(w.benedictionHealFromParams({
            targetMaxHP = 1000, casterMainLvl = 99, targetMainLvl = 50,
        }) == 1980)
        assert(w.benedictionHealFromParams({
            targetMaxHP = 1000, casterMainLvl = 50, targetMainLvl = 99,
        }) == 505)
        assert(w.benedictionHealFromParams({
            targetMaxHP = 1000, casterMainLvl = 75, targetMainLvl = 0,
        }) == 0)

        assert(w.benedictionHealClamped(1980, 900, 1000) == 100)
        assert(w.benedictionHealClamped(50, 900, 1000) == 50)
        assert(w.benedictionHealClamped(50, 1000, 1000) == 0)

        assert(w.benedictionRemovesDoom(1) and w.benedictionRemovesDoom(32))
        assert(not w.benedictionRemovesDoom(33) and not w.benedictionRemovesDoom(100))
    end)
end)

describe('checkDevotion Martyr', function()
    it('self and low HP gates', function()
        assert(w.checkDevotionFromParams({ actorID = 1, targetID = 1, actorHP = 100 }) == 72)
        assert(w.checkDevotionFromParams({ actorID = 1, targetID = 2, actorHP = 3 }) == 87)
        assert(w.checkDevotionFromParams({ actorID = 1, targetID = 2, actorHP = 4 }) == 0)
        assert(w.checkMartyrFromParams({ actorID = 5, targetID = 5, actorHP = 50 }) == 72)
        assert(w.checkMartyrFromParams({ actorID = 5, targetID = 6, actorHP = 1 }) == 87)
        assert(w.checkMartyrFromParams({ actorID = 5, targetID = 6, actorHP = 100 }) == 0)
    end)
end)

describe('devotion and martyr products', function()
    it('percent damage heal clamps', function()
        assert(almost(w.devotionMPPercent(5), 0.25))
        assert(almost(w.devotionMPPercent(25), 0.45))
        assert(w.devotionDamageHP(1000) == 250)
        assert(w.devotionDamageHP(999) == 249)
        assert(almost(w.devotionHealMP(1000, 0.25), 250))
        assert(almost(w.devotionHealMPClamped(250, 900, 1000), 100))
        assert(almost(w.devotionHealMPClamped(50, 900, 1000), 50))
        assert(almost(w.devotionHealMPClamped(50, 1000, 1000), 0))

        assert(almost(w.martyrHPPercent(5), 2.0))
        assert(almost(w.martyrHPPercent(25), 2.2))
        assert(w.martyrDamageHP(800) == 200)
        assert(almost(w.martyrHealHP(200, 2.0), 400))
        assert(almost(w.martyrHealHP(200, 2.2), 440))
        assert(almost(w.martyrHealHPClamped(440, 800, 1000), 200))
        assert(almost(w.martyrHealHPClamped(50, 800, 1000), 50))
    end)
end)

describe('oneHourRecastFromParams', function()
    it('reduction floor', function()
        assert(w.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(w.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(w.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)
    end)
end)
