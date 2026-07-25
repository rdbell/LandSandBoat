-----------------------------------
-- Pure system tests for Fencer gear eligibility and critrate pure injects
-- (slices 6212 / 6686). Calls production xi.combat.physical pure exports.
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical

describe('critrate fencer eligible pure plan', function()
    it('one-hand + empty/shield/none sub is eligible', function()
        assert(phys.fencerEligible(true, true, false, false, false, 0, false))
        assert(phys.fencerEligible(true, true, false, false, true, 0, false))
        assert(phys.fencerEligible(true, true, false, false, true, 32, true))
    end)

    it('two-hand, H2H, dual wield, non-PC are not', function()
        assert(not phys.fencerEligible(true, true, true, false, false, 0, false))
        assert(not phys.fencerEligible(true, true, false, true, false, 0, false))
        assert(not phys.fencerEligible(true, true, false, false, true, 3, false))
        assert(not phys.fencerEligible(false, true, false, false, false, 0, false))
        assert(not phys.fencerEligible(true, false, false, false, false, 0, false))
    end)
end)

describe('critrate from fencer value', function()
    it('returns mod/100 only when eligible', function()
        assert(phys.criticalRateFromFencerValue(false, 50) == 0)
        assert(math.abs(phys.criticalRateFromFencerValue(true, 50) - 0.5) < 1e-12)
        assert(phys.criticalRateFromFencerValue(true, 0) == 0)
    end)
end)

describe('critrate from stat / AGI / innin / flourish / weapon slot', function()
    it('dDEX bands', function()
        assert(phys.criticalRateFromStatDiffValue(6) == 0)
        assert(phys.criticalRateFromStatDiffValue(7) == 0.01)
        assert(phys.criticalRateFromStatDiffValue(14) == 0.02)
        assert(phys.criticalRateFromStatDiffValue(20) == 0.03)
        assert(phys.criticalRateFromStatDiffValue(30) == 0.04)
        assert(math.abs(phys.criticalRateFromStatDiffValue(40) - 0.05) < 1e-12) -- (40-35)/100
        assert(math.abs(phys.criticalRateFromStatDiffValue(45) - 0.10) < 1e-12)
        -- dDex >= 40: (dDex-35)/100; at 50 → 0.15; dDex > 50 → hard 0.15
        assert(math.abs(phys.criticalRateFromStatDiffValue(50) - 0.15) < 1e-12)
        assert(phys.criticalRateFromStatDiffValue(51) == 0.15)
    end)

    it('dAGI floor bands', function()
        assert(phys.criticalRateFromAGIDiffValue(-5) == 0)
        assert(phys.criticalRateFromAGIDiffValue(0) == 0)
        assert(phys.criticalRateFromAGIDiffValue(9) == 0)
        assert(phys.criticalRateFromAGIDiffValue(10) == 0.01)
        assert(phys.criticalRateFromAGIDiffValue(25) == 0.02)
    end)

    it('innin requires status and behind', function()
        assert(phys.criticalRateFromInninValue(false, true, 10) == 0)
        assert(phys.criticalRateFromInninValue(true, false, 10) == 0)
        assert(phys.criticalRateFromInninValue(true, true, 10) == 10)
    end)

    it('flourish requires power >= 3', function()
        assert(phys.criticalRateFromFlourishValue(false, 3, 5) == 0)
        assert(phys.criticalRateFromFlourishValue(true, 2, 5) == 0)
        assert(math.abs(phys.criticalRateFromFlourishValue(true, 3, 5) - 0.15) < 1e-12) -- (10+5)/100
    end)

    it('weapon slot divides mod by 100', function()
        assert(phys.criticalRateFromWeaponSlotValue(0) == 0)
        assert(math.abs(phys.criticalRateFromWeaponSlotValue(25) - 0.25) < 1e-12)
    end)
end)

describe('critrate clamp', function()
    it('clamps to [0.05, 1]', function()
        assert(phys.clampCriticalRate(0) == 0.05)
        assert(phys.clampCriticalRate(0.05) == 0.05)
        assert(phys.clampCriticalRate(0.5) == 0.5)
        assert(phys.clampCriticalRate(1.0) == 1.0)
        assert(phys.clampCriticalRate(1.5) == 1.0)
    end)
end)
