-----------------------------------
-- Pure system tests for weaponskill WSC dual-wire helpers (slice 6685).
-- Calls production xi.combat.physical.wscTerm / wscFromParams.
-- Goldens match internal/wsc (0842). Distinct from blue WSC (no per-stat floor).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical

describe('WSC term', function()
    it('floors each product toward -inf', function()
        assert(phys.wscTerm(100, 0.3, 0) == 30)
        assert(phys.wscTerm(99, 0.3, 0) == 29) -- 29.7
        assert(phys.wscTerm(100, 0.3, 20) == 50) -- 100 * 0.5
        assert(phys.wscTerm(10, -0.35, 0) == -4) -- -3.5 → -4
        assert(phys.wscTerm(nil, nil, nil) == 0)
    end)
end)

describe('WSC from params', function()
    it('returns 0 for zero multipliers', function()
        assert(phys.wscFromParams({
            stats = { str = 100, dex = 90, vit = 80, agi = 70, int = 60, mnd = 50, chr = 40 },
        }) == 0)
    end)

    it('computes single-stat WSC', function()
        -- 60% STR: floor(120 * 0.6) = 72
        assert(phys.wscFromParams({
            stats = { str = 120 },
            multipliers = { str = 0.6 },
        }) == 72)

        -- 40% DEX: floor(95 * 0.4) = 38
        assert(phys.wscFromParams({
            stats = { dex = 95 },
            multipliers = { dex = 0.4 },
        }) == 38)
    end)

    it('sums multi-stat terms independently floored', function()
        -- floor(100*0.3)+floor(80*0.2)+floor(70*0.1)=30+16+7=53
        assert(phys.wscFromParams({
            stats = { str = 100, dex = 80, vit = 70, agi = 50, int = 40, mnd = 30, chr = 20 },
            multipliers = { str = 0.3, dex = 0.2, vit = 0.1 },
        }) == 53)

        -- Without per-term floor 59.4→59; with independent floors 29+29=58
        assert(phys.wscFromParams({
            stats = { str = 99, dex = 99 },
            multipliers = { str = 0.3, dex = 0.3 },
        }) == 58)
    end)

    it('adds WS_*_BONUS percents into multipliers', function()
        -- Script 0.4 STR + bonus 25 → 0.65; floor(200*0.65)=130
        assert(phys.wscFromParams({
            stats = { str = 200 },
            multipliers = { str = 0.4 },
            bonusPercents = { str = 25 },
        }) == 130)

        -- STR: floor(100*0.3)=30; MND: floor(100*0.35)=35; DEX: floor(999*0.5)=499 → 564
        assert(phys.wscFromParams({
            stats = { str = 100, mnd = 100, dex = 999 },
            multipliers = { str = 0.2, mnd = 0.3 },
            bonusPercents = { str = 10, mnd = 5, dex = 50 },
        }) == 564)
    end)

    it('sums all seven stats', function()
        -- 10 each * 0.1 = floor(1) each → 7
        assert(phys.wscFromParams({
            stats = { str = 10, dex = 10, vit = 10, agi = 10, int = 10, mnd = 10, chr = 10 },
            multipliers = { str = 0.1, dex = 0.1, vit = 0.1, agi = 0.1, int = 0.1, mnd = 0.1, chr = 0.1 },
        }) == 7)
    end)
end)
