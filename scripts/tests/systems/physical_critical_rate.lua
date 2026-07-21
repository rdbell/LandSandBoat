-----------------------------------
-- Pure system tests for physical critical rate injects (slice 6079).
-- Mirrors xi.combat.physical.criticalRateFrom* pure halves.
-----------------------------------

describe('physical critical rate pure injects', function()
    local function almostEqual(a, b)
        return math.abs(a - b) < 1e-9
    end

    local function fromStatDiff(dDex)
        if dDex > 50 then return 0.15
        elseif dDex >= 40 then return (dDex - 35) / 100
        elseif dDex >= 30 then return 0.04
        elseif dDex >= 20 then return 0.03
        elseif dDex >= 14 then return 0.02
        elseif dDex >= 7 then return 0.01
        end
        return 0
    end

    local function fromAGIDiff(dAgi)
        if dAgi < 0 then dAgi = 0 end
        return math.floor(dAgi / 10) / 100
    end

    local function clampRate(v)
        if v < 0.05 then return 0.05 end
        if v > 1 then return 1 end
        return v
    end

    local function swingRate(statBonus, extras)
        extras = extras or 0
        return clampRate(0.05 + statBonus + extras)
    end

    it('dDEX fractional bands', function()
        assert(fromStatDiff(0) == 0)
        assert(almostEqual(fromStatDiff(7), 0.01))
        assert(almostEqual(fromStatDiff(14), 0.02))
        assert(almostEqual(fromStatDiff(20), 0.03))
        assert(almostEqual(fromStatDiff(30), 0.04))
        assert(almostEqual(fromStatDiff(40), 0.05))
        assert(almostEqual(fromStatDiff(50), 0.15))
        assert(almostEqual(fromStatDiff(51), 0.15))
    end)

    it('dAGI fractional bonus', function()
        assert(fromAGIDiff(0) == 0)
        assert(almostEqual(fromAGIDiff(10), 0.01))
        assert(almostEqual(fromAGIDiff(25), 0.02))
    end)

    it('swing rate clamp', function()
        assert(almostEqual(swingRate(0), 0.05))
        assert(almostEqual(swingRate(0.10), 0.15))
        assert(almostEqual(swingRate(0, 2), 1))
        assert(almostEqual(swingRate(0, -1), 0.05))
    end)
end)
