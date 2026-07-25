-----------------------------------
-- Pure system tests for pDIF curve dual-wire helpers (slice 6684).
-- Calls production xi.combat.physical pure exports (weaponCap, calculateTPfactor,
-- wRatioCapPC, wRatioCapOthers). Goldens match internal/pdif (0840).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical

local q = function(n)
    return n / 1024
end

describe('pDIF weaponCap', function()
    it('returns table values and default for unknown skills', function()
        assert(phys.defaultWeaponCap == 3)
        assert(phys.weaponCap(xi.skill.NONE) == 3)
        assert(phys.weaponCap(xi.skill.HAND_TO_HAND) == 3.5)
        assert(phys.weaponCap(xi.skill.DAGGER) == 3.25)
        assert(phys.weaponCap(xi.skill.SWORD) == 3.25)
        assert(phys.weaponCap(xi.skill.GREAT_SWORD) == 3.75)
        assert(phys.weaponCap(xi.skill.SCYTHE) == 4)
        assert(phys.weaponCap(xi.skill.ARCHERY) == 3.25)
        assert(phys.weaponCap(xi.skill.MARKSMANSHIP) == 3.5)
        assert(phys.weaponCap(xi.skill.THROWING) == 3.25)
        -- Guard / Shield / out-of-table fall back to 3
        assert(phys.weaponCap(28) == 3)
        assert(phys.weaponCap(30) == 3)
        assert(phys.weaponCap(255) == 3)
    end)
end)

describe('pDIF calculateTPfactor', function()
    it('interpolates 1000/2000/3000 segments', function()
        local tab = { 1.0, 2.0, 3.0 }
        assert(phys.calculateTPfactor(0, tab) == 1.0)
        assert(phys.calculateTPfactor(999, tab) == 1.0)
        assert(phys.calculateTPfactor(1000, tab) == 1.0)
        assert(math.abs(phys.calculateTPfactor(1500, tab) - 1.5) < 1e-12)
        assert(phys.calculateTPfactor(2000, tab) == 2.0)
        assert(math.abs(phys.calculateTPfactor(2500, tab) - 2.5) < 1e-12)
        assert(phys.calculateTPfactor(3000, tab) == 3.0)
        assert(math.abs(phys.calculateTPfactor(3500, tab) - 3.5) < 1e-12)
    end)

    it('returns 0 for nil table and handles decreasing segments', function()
        assert(phys.calculateTPfactor(2500, nil) == 0)
        local dec = { 3.0, 2.0, 1.0 }
        assert(math.abs(phys.calculateTPfactor(1500, dec) - 2.5) < 1e-12)
        assert(math.abs(phys.calculateTPfactor(2500, dec) - 1.5) < 1e-12)
    end)
end)

describe('pDIF wRatioCapPC', function()
    local finalCap = 3.25

    it('pins upper and lower bands', function()
        local lo, hi = phys.wRatioCapPC(0.25, finalCap)
        assert(lo == 0 and math.abs(hi - 0.75) < 1e-12)

        lo, hi = phys.wRatioCapPC(0.6, finalCap)
        assert(math.abs(lo - (0.6 * q(1176) - q(448))) < 1e-12)
        assert(hi == 1)

        lo, hi = phys.wRatioCapPC(1.0, finalCap)
        assert(math.abs(lo - (1.0 * q(1176) - q(448))) < 1e-12)
        assert(math.abs(hi - 1.3) < 1e-12)

        lo, hi = phys.wRatioCapPC(1.3, finalCap)
        assert(lo == 1)
        assert(math.abs(hi - (1.3 + 1.3 * 0.25)) < 1e-12)

        lo, hi = phys.wRatioCapPC(2.0, finalCap)
        assert(math.abs(lo - (2.0 * q(1176) - q(775))) < 1e-12)
        assert(math.abs(hi - math.min(2.0 + 0.375, finalCap)) < 1e-12)

        lo, hi = phys.wRatioCapPC(5.0, finalCap)
        assert(math.abs(lo - math.min(5.0 - 0.375, finalCap)) < 1e-12)
        assert(math.abs(hi - math.min(5.0 + 0.375, finalCap)) < 1e-12)
    end)

    it('keeps lower <= upper on typical wRatios', function()
        for _, w in ipairs({ 0, 0.2, 0.38, 0.5, 0.7, 1.0, 1.2, 1.25, 1.5, 1.51, 2.0, 2.44, 3.0, 4.0 }) do
            local lo, hi = phys.wRatioCapPC(w, 3.5)
            assert(lo <= hi + 1e-9, 'wRatio ' .. w)
        end
    end)
end)

describe('pDIF wRatioCapOthers', function()
    local finalCap = 4.0

    it('pins non-PC band formulas', function()
        local lo, hi = phys.wRatioCapOthers(0.25, finalCap)
        assert(lo == 0.25)
        assert(math.abs(hi - (0.6 + q(760) * 0.25)) < 1e-12)

        lo, hi = phys.wRatioCapOthers(0.7, finalCap)
        assert(math.abs(lo - (0.25 + q(827) * (0.7 - 0.4))) < 1e-12)
        assert(hi == 1)

        lo, hi = phys.wRatioCapOthers(1.0, finalCap)
        assert(math.abs(lo - (0.25 + q(827) * (1.0 - 0.4))) < 1e-12)
        assert(math.abs(hi - (1 + q(1127) * (1.0 - 0.8))) < 1e-12)

        lo, hi = phys.wRatioCapOthers(2.0, finalCap)
        assert(math.abs(lo - math.min(1 + q(1120) * (2.0 - 1.59), finalCap)) < 1e-12)
        assert(math.abs(hi - math.min(q(1803) + q(1070) * (2.0 - 1.5), finalCap)) < 1e-12)
    end)

    it('uses <= boundaries at 0.8 and 1.60', function()
        local lo, hi = phys.wRatioCapOthers(0.8, 3.0)
        assert(hi == 1)
        assert(math.abs(lo - (0.25 + q(827) * (0.8 - 0.4))) < 1e-12)

        lo, hi = phys.wRatioCapOthers(1.60, 3.0)
        assert(lo == 1)
    end)
end)
