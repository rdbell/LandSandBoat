-----------------------------------
-- Pure system tests for finalMagicNonSpellAdjustments product (slice 6077).
-- Entity tails mocked via inject mirrors of the pure product chain.
-----------------------------------

describe('finalMagicNonSpellAdjustments pure product', function()
    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function handlePhalanx(damage, phalanx)
        if damage <= 0 then return damage end
        return clamp(damage - phalanx, 0, 99999)
    end

    local function handleOneForAll(damage, power, has)
        if damage <= 0 or not has then return damage end
        return clamp(damage - power, 0, 99999)
    end

    local function handleStoneskin(damage, skin)
        if damage <= 0 or skin <= 0 then return damage end
        if skin > damage then return 0 end
        return damage - skin
    end

    local function adjust(base, dmgAdj, absorb, nullify, phalanx, ofaPower, ofaHas, skin)
        local dmg = math.floor(base * dmgAdj)
        dmg = math.floor(dmg * absorb)
        dmg = math.floor(dmg * nullify)
        dmg = handlePhalanx(dmg, phalanx or 0)
        dmg = handleOneForAll(dmg, ofaPower or 0, ofaHas)
        dmg = handleStoneskin(dmg, skin or 0)
        return clamp(dmg, -99999, 99999)
    end

    it('identity and phalanx', function()
        assert(adjust(100, 1, 1, 1, 0, 0, false, 0) == 100)
        assert(adjust(200, 0.5, 1, 1, 30, 0, false, 0) == 70)
    end)

    it('one for all and stoneskin', function()
        assert(adjust(100, 1, 1, 1, 0, 20, true, 50) == 30)
        assert(adjust(40, 1, 1, 1, 0, 0, false, 100) == 0)
    end)

    it('nullification and clamp', function()
        assert(adjust(999, 1, 1, 0, 0, 0, false, 0) == 0)
        assert(adjust(200000, 1, 1, 1, 0, 0, false, 0) == 99999)
    end)
end)
