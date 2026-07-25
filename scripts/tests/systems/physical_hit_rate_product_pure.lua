-----------------------------------
-- Pure system tests for melee/ranged hit-rate full product dual-wire (slice 6763).
-- Calls production xi.combat.physicalHitRate pure exports.
-- Goldens match internal/physhitrate MeleeHitRate / RangedHitRate.
-----------------------------------

require('scripts/globals/combat/physical_hit_rate')

local phr = xi.combat.physicalHitRate

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('meleeHitRateFromParams', function()
    it('even ACC/EVA yields 0.75 under mainhand cap', function()
        assert(phr.meleeHitRateFromParams({
            acc = 100, eva = 100, cap = phr.capPCMainhand1H,
        }) == 0.75)
    end)

    it('floors at meleeHitRateFloor', function()
        assert(phr.meleeHitRateFromParams({
            acc = 0, eva = 200, cap = phr.capPCMainhand1H,
        }) == phr.meleeHitRateFloor)
    end)

    it('caps at offhand/2H cap', function()
        assert(phr.meleeHitRateFromParams({
            acc = 300, eva = 0, cap = phr.capPCOffhandOr2H,
        }) == phr.capPCOffhandOr2H)
    end)

    it('stacks bonus and accBonus before formula', function()
        -- 100 + 10 + 10 = 120 vs 100 → 0.85
        assert(almost(phr.meleeHitRateFromParams({
            acc = 100, eva = 100, bonus = 10, accBonus = 10,
            cap = phr.capPCMainhand1H,
        }), 0.85))
    end)

    it('applies PC level penalty inject', function()
        -- ACC 100, levels 40 vs 50 → ACC 60 → 0.55
        assert(almost(phr.meleeHitRateFromParams({
            acc = 100, eva = 100, cap = phr.capPCMainhand1H,
            applyLevelCorrection = true,
            attackerLevel = 40, defenderLevel = 50,
            attackerIsPC = true,
        }), 0.55))
    end)

    it('applies non-PC level bonus inject', function()
        -- ACC 100, levels 60 vs 50 → ACC 140 → 0.95
        assert(almost(phr.meleeHitRateFromParams({
            acc = 100, eva = 100, cap = phr.capNonPC,
            applyLevelCorrection = true,
            attackerLevel = 60, defenderLevel = 50,
            attackerIsPC = false,
        }), 0.95))
    end)
end)

describe('rangedHitRateFromParams', function()
    it('short-circuits when distance > maxRangedDistance', function()
        assert(phr.rangedHitRateFromParams({
            acc = 200, eva = 0, distance = 25.01,
        }) == 0)
    end)

    it('allows distance exactly maxRangedDistance', function()
        assert(phr.rangedHitRateFromParams({
            acc = 100, eva = 100, distance = 25,
        }) == 0.75)
    end)

    it('subtracts distance penalty from ACC', function()
        -- 100 - 20 = 80 vs 100 → 0.65
        assert(almost(phr.rangedHitRateFromParams({
            acc = 100, eva = 100, distancePenalty = 20, distance = 10,
        }), 0.65))
    end)

    it('floors and caps ranged hit rate', function()
        assert(phr.rangedHitRateFromParams({
            acc = 0, eva = 500, distance = 1,
        }) == phr.rangedHitRateFloor)
        assert(phr.rangedHitRateFromParams({
            acc = 500, eva = 0, distance = 1,
        }) == phr.rangedHitRateCap)
    end)

    it('stacks bonus', function()
        assert(almost(phr.rangedHitRateFromParams({
            acc = 100, eva = 100, bonus = 20, distance = 5,
        }), 0.85))
    end)
end)
