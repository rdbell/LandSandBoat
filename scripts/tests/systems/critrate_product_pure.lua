-----------------------------------
-- Pure system tests for critical-rate full product dual-wire (slice 6764).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/critrate SwingRate / RangedRate.
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('criticalRateFromParams', function()
    it('base rate with no bonuses is 0.05', function()
        assert(p.criticalRateFromParams({}) == 0.05)
        assert(p.criticalRateFromParams({
            statBonus = 0, inninBonus = 0, fencerBonus = 0,
        }) == p.baseCriticalRate)
    end)

    it('sums bonuses and subtracts target penalties', function()
        -- 0.05 + 0.15 + 0.10 - 0.05 - 0.05 + 0.20 = 0.40
        assert(almost(p.criticalRateFromParams({
            statBonus     = 0.15,
            inninBonus    = 0.10,
            targetEvasion = 0.05,
            targetMerit   = 0.05,
            tpFactor      = 0.20,
        }), 0.40))
    end)

    it('floors at criticalRateFloor', function()
        assert(p.criticalRateFromParams({
            targetEvasion = 1.0,
        }) == p.criticalRateFloor)
    end)

    it('caps at criticalRateCap', function()
        assert(p.criticalRateFromParams({
            statBonus     = 0.5,
            modifierBonus = 0.5,
            meritBonus    = 0.5,
        }) == p.criticalRateCap)
    end)

    it('matches swing path with dDEX inject', function()
        local stat = p.criticalRateFromStatDiffValue(45) -- (45-35)/100 = 0.10
        assert(almost(p.criticalRateFromParams({
            statBonus = stat,
        }), 0.15))
    end)

    it('matches ranged path with dAGI inject', function()
        local stat = p.criticalRateFromAGIDiffValue(25) -- floor(25/10)/100 = 0.02
        assert(almost(p.criticalRateFromParams({
            statBonus = stat,
        }), 0.07))
    end)
end)
