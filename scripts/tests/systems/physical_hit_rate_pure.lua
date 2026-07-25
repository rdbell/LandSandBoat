-----------------------------------
-- Pure system tests for physical hit rate dual-wire helpers (slice 6681).
-- Calls production xi.combat.physicalHitRate pure exports.
-- Goldens match internal/physhitrate (0838).
-----------------------------------

require('scripts/globals/combat/physical_hit_rate')

local phr = xi.combat.physicalHitRate

describe('Physical hit rate constants', function()
    it('pins caps floors and Flash scale', function()
        assert(phr.capPet == 0.99)
        assert(phr.capPCH2H == 0.99)
        assert(phr.capPCMainhand1H == 0.99)
        assert(phr.capPCOffhandOr2H == 0.95)
        assert(phr.capNonPC == 0.95)
        assert(phr.meleeHitRateFloor == 0.2)
        assert(phr.rangedHitRateFloor == 0.05)
        assert(phr.rangedHitRateCap == 0.95)
        assert(phr.maxRangedDistance == 25)
        assert(phr.flashReductionPerMs == 0.03)
        assert(phr.baseHitPercent == 75)
        assert(phr.avatarDlvlMax == 38)
    end)
end)

describe('Physical hit rate hitRateCap', function()
    it('selects pet / PC / non-PC caps', function()
        assert(phr.hitRateCap(true, false, false, false, false) == 0.99)
        assert(phr.hitRateCap(true, true, true, true, true) == 0.99) -- pet first
        assert(phr.hitRateCap(false, true, true, false, false) == 0.99) -- PC H2H
        assert(phr.hitRateCap(false, true, true, true, false) == 0.99) -- H2H wins 2H
        assert(phr.hitRateCap(false, true, false, true, false) == 0.95) -- PC 2H
        assert(phr.hitRateCap(false, true, false, false, true) == 0.95) -- PC offhand/left
        assert(phr.hitRateCap(false, true, false, false, false) == 0.99) -- PC 1H main
        assert(phr.hitRateCap(false, false, true, true, true) == 0.95) -- mob
    end)
end)

describe('Physical hit rate flashPenalty', function()
    it('floors ms * 0.03 and zeros non-positive remaining', function()
        assert(phr.flashPenalty(0) == 0)
        assert(phr.flashPenalty(-100) == 0)
        assert(phr.flashPenalty(nil) == 0)
        -- 12s flash → floor(12000 * 0.03) = 360
        assert(phr.flashPenalty(12000) == 360)
        assert(phr.flashPenalty(1000) == 30)
        assert(phr.flashPenalty(1) == 0)
        assert(phr.flashPenalty(34) == 1) -- 1.02
        assert(phr.flashPenalty(33) == 0) -- 0.99
    end)
end)

describe('Physical hit rate levelCorrectedAccuracy', function()
    it('passes through when zone not level-corrected', function()
        assert(phr.levelCorrectedAccuracy(100, 75, 50, false, false, false) == 100)
    end)

    it('gives non-PC higher-level ACC bonus', function()
        -- dlvl 10 → +40
        assert(phr.levelCorrectedAccuracy(100, 60, 50, true, false, false) == 140)
        assert(phr.levelCorrectedAccuracy(100, 40, 50, true, false, false) == 100)
    end)

    it('gives PC lower-level ACC penalty only', function()
        assert(phr.levelCorrectedAccuracy(100, 40, 50, true, true, false) == 60)
        assert(phr.levelCorrectedAccuracy(100, 60, 50, true, true, false) == 100)
    end)

    it('clamps avatar dlvl to [0, 38]', function()
        assert(phr.levelCorrectedAccuracy(0, 99, 49, true, false, true) == 38 * 4)
        -- negative raw dlvl clamped to 0 → no bonus
        assert(phr.levelCorrectedAccuracy(100, 40, 50, true, false, true) == 100)
    end)

    it('leaves equal levels unchanged', function()
        assert(phr.levelCorrectedAccuracy(200, 75, 75, true, false, false) == 200)
    end)
end)

describe('Physical hit rate accuracyEvasionToHitRate', function()
    it('uses (75 + (acc-eva)/2) / 100 unclamped', function()
        assert(phr.accuracyEvasionToHitRate(100, 100) == 0.75)
        assert(math.abs(phr.accuracyEvasionToHitRate(120, 100) - 0.85) < 1e-12)
        assert(math.abs(phr.accuracyEvasionToHitRate(80, 100) - 0.65) < 1e-12)
        assert(math.abs(phr.accuracyEvasionToHitRate(101, 100) - 0.755) < 1e-12)
        assert(math.abs(phr.accuracyEvasionToHitRate(200, 0) - 1.75) < 1e-12)
        assert(math.abs(phr.accuracyEvasionToHitRate(0, 200) - (-0.25)) < 1e-12)
    end)
end)

describe('Physical hit rate clamps', function()
    it('clamps melee to [0.2, cap] and ranged to [0.05, 0.95]', function()
        assert(phr.clampMeleeHitRate(0.10, 0.99) == 0.2)
        assert(phr.clampMeleeHitRate(0.99, 0.95) == 0.95)
        assert(phr.clampMeleeHitRate(0.50, 0.99) == 0.50)
        assert(phr.clampRangedHitRate(0.01) == 0.05)
        assert(phr.clampRangedHitRate(0.99) == 0.95)
        assert(phr.clampRangedHitRate(0.50) == 0.50)
    end)
end)

describe('Physical hit rate pure composition', function()
    it('composes level correction then hitdiff then melee clamp', function()
        -- non-PC +10 levels: acc 100 → 140; eva 100 → hitdiff 20 → 0.95; cap 0.95
        local acc = phr.levelCorrectedAccuracy(100, 60, 50, true, false, false)
        assert(acc == 140)
        local rate = phr.accuracyEvasionToHitRate(acc, 100)
        assert(math.abs(rate - 0.95) < 1e-12)
        assert(phr.clampMeleeHitRate(rate, phr.capNonPC) == 0.95)
    end)
end)
