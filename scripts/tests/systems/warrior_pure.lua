-----------------------------------
-- Pure system tests for Warrior dual-wire (slice 6730).
-- Calls production xi.job_utils.warrior pure exports.
-- Goldens match internal/warrior (0888).
-----------------------------------

require('scripts/globals/job_utils/warrior')

local w = xi.job_utils.warrior

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Warrior pure pins', function()
    it('bases and offsets', function()
        assert(w.berserkBasePower == 25)
        assert(w.defenderBasePower == 25)
        assert(w.bloodRageBasePower == 20)
        assert(w.bloodRageBaseDuration == 30)
        assert(w.standardStanceDuration == 180)
        assert(w.warcryBaseDuration == 30)
        assert(w.tomahawkBaseDuration == 30)
        assert(w.tomahawkMeritOffset == 15)
        assert(w.warriorsChargeMeritOffset == 5)
        assert(w.warriorsChargeDuration == 60)
        assert(w.levelScaleFloor == 40)
        assert(w.levelScaleCap == 10)
        assert(w.itemThrowingTomahawk == 18258)
        assert(w.msgCannotPerform == 71)
        assert(w.tomahawkPower == 25)
    end)
end)

describe('levelScaleContribution', function()
    it('main-WAR ladder and clamp', function()
        assert(w.levelScaleContribution(false, 99) == 0)
        assert(w.levelScaleContribution(true, 40) == 0)
        assert(w.levelScaleContribution(true, 49) == 0)
        assert(w.levelScaleContribution(true, 50) == 2)
        assert(w.levelScaleContribution(true, 60) == 4)
        assert(w.levelScaleContribution(true, 90) == 10)
        assert(w.levelScaleContribution(true, 99) == 10)
        assert(w.levelScaleContribution(true, 30) == 0)
    end)
end)

describe('Berserk Defender Aggressor BloodRage', function()
    it('power and duration products', function()
        assert(w.berserkPowerFromParams({ mainJobIsWAR = true, mainLevel = 99 }) == 35)
        assert(w.berserkPowerFromParams({ mainJobIsWAR = false, mainLevel = 99 }) == 25)
        assert(w.berserkPowerFromParams({
            mainJobIsWAR = true, mainLevel = 50, berserkPotencyMod = 5,
        }) == 32)
        assert(w.berserkDurationFromParams({}) == 180)
        assert(w.berserkDurationFromParams({ berserkDurationMod = 30 }) == 210)

        assert(w.defenderPowerFromParams({ mainJobIsWAR = true, mainLevel = 99 }) == 35)
        assert(w.defenderPowerFromParams({ mainJobIsWAR = false, mainLevel = 99 }) == 25)
        assert(w.defenderPowerFromParams({ mainJobIsWAR = true, mainLevel = 50 }) == 27)
        assert(w.defenderDurationFromParams({ defenderDurationMod = 15 }) == 195)

        assert(w.aggressorPowerFromParams({ aggressiveAimMerit = 25 }) == 25)
        assert(w.aggressorDurationFromParams({}) == 180)
        assert(w.aggressorDurationFromParams({ aggressorDurationMod = 20 }) == 200)

        assert(w.bloodRagePowerFromParams({}) == 20)
        assert(w.bloodRagePowerFromParams({ bloodRageJP = 10 }) == 30)
        assert(w.bloodRageDurationFromParams({}) == 30)
        assert(w.bloodRageDurationFromParams({ enhancesBloodRageMod = 15 }) == 45)
    end)
end)

describe('Warcry Tomahawk Charge', function()
    it('formulas', function()
        assert(almost(w.warcryPowerFromParams({ warLevel = 1 }), (5 / 256) * 100))
        assert(almost(w.warcryPowerFromParams({ warLevel = 75 }), (23 / 256) * 100))
        assert(almost(w.warcryPowerFromParams({ warLevel = 99 }), (29 / 256) * 100))
        assert(almost(w.warcryPowerFromParams({ warLevel = 0 }), (4 / 256) * 100))
        assert(w.warcryDurationFromParams({}) == 30)
        assert(w.warcryDurationFromParams({ warcryDurationMod = 12 }) == 42)
        assert(w.warcrySubPowerFromParams({ savageryMerit = 25 }) == 25)

        assert(w.tomahawkDurationFromParams({ tomahawkMerit = 15 }) == 30)
        assert(w.tomahawkDurationFromParams({ tomahawkMerit = 25 }) == 40)
        assert(w.warriorsChargePowerFromParams({ warriorsChargeMerit = 10 }) == 5)
        assert(w.warriorsChargePowerFromParams({ warriorsChargeMerit = 5 }) == 0)
    end)
end)

describe('oneHourRecast checkTomahawk partyTarget activeJobLevel', function()
    it('gates and helpers', function()
        assert(w.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(w.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(w.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)

        assert(w.checkTomahawkFromParams({ ammoID = w.itemThrowingTomahawk }) == 0)
        assert(w.checkTomahawkFromParams({ ammoID = 0 }) == 71)
        assert(w.checkTomahawkFromParams({ ammoID = 1 }) == 71)

        local msg, changed = w.partyTargetMessage(1, 1, 266)
        assert(not changed and msg == 0)
        msg, changed = w.partyTargetMessage(1, 2, 266)
        assert(changed and msg == 266)
        msg, changed = w.partyTargetMessage(1, 2, 285)
        assert(changed and msg == 285)

        assert(w.activeJobLevel(xi.job.WAR, 2, xi.job.WAR, 75, 37) == 75)
        assert(w.activeJobLevel(2, xi.job.WAR, xi.job.WAR, 75, 37) == 37)
        assert(w.activeJobLevel(2, 3, xi.job.WAR, 75, 37) == 0)
    end)
end)
