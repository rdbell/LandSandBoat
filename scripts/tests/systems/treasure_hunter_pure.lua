-----------------------------------
-- Pure system tests for Treasure Hunter drop-rate dual-wire (slice 6694).
-- Calls production xi.combat.treasureHunter pure exports.
-- Goldens match internal/treasurehunter (0834 / 1585).
-----------------------------------

require('scripts/globals/combat/treasure_hunter')

local th = xi.combat.treasureHunter

describe('treasure hunter constants and tables', function()
    it('pins max tier, scale, and bracket count', function()
        assert(th.maxTier == 14)
        assert(th.maxDropRate == 10000)
        assert(th.bracketCount == 7)
    end)

    it('pins TH0 VC and TH14 UR table corners', function()
        assert(th.treasureHunterTable[0][1] == 2400)
        assert(th.treasureHunterTable[0][7] == 10)
        assert(th.treasureHunterTable[14][1] == 8000)
        assert(th.treasureHunterTable[14][7] == 150)
        assert(th.treasureHunterTable[5][1] == 6666)
    end)

    it('pins drop bracket thresholds', function()
        assert(th.dropBracketTable[1][1] == 2400)
        assert(th.dropBracketTable[2][1] == 1500)
        assert(th.dropBracketTable[3][1] == 1000)
        assert(th.dropBracketTable[4][1] == 500)
        assert(th.dropBracketTable[5][1] == 100)
        assert(th.dropBracketTable[6][1] == 50)
        assert(th.dropBracketTable[7][1] == 0)
    end)
end)

describe('clampTier and clampDropRate', function()
    it('clamps tier to 0..14 and rate to 0..10000', function()
        assert(th.clampTier(-5) == 0)
        assert(th.clampTier(0) == 0)
        assert(th.clampTier(14) == 14)
        assert(th.clampTier(99) == 14)
        assert(th.clampDropRate(-1) == 0)
        assert(th.clampDropRate(0) == 0)
        assert(th.clampDropRate(10000) == 10000)
        assert(th.clampDropRate(10001) == 10000)
    end)
end)

describe('dropBracket', function()
    it('first-matches descending rarity floors', function()
        assert(th.dropBracket(10000) == 1)
        assert(th.dropBracket(2400) == 1)
        assert(th.dropBracket(2399) == 2)
        assert(th.dropBracket(1500) == 2)
        assert(th.dropBracket(1499) == 3)
        assert(th.dropBracket(1000) == 3)
        assert(th.dropBracket(999) == 4)
        assert(th.dropBracket(500) == 4)
        assert(th.dropBracket(499) == 5)
        assert(th.dropBracket(100) == 5)
        assert(th.dropBracket(99) == 6)
        assert(th.dropBracket(50) == 6)
        assert(th.dropBracket(49) == 7)
        assert(th.dropBracket(1) == 7)
        -- rate 0 is clamped; first-match still hits terminal 0 → bracket 7
        assert(th.dropBracket(0) == 7)
    end)
end)

describe('getDropRate', function()
    it('short-circuits guaranteed and zero drops', function()
        assert(th.getDropRate(0, 10000) == 10000)
        assert(th.getDropRate(14, 10000) == 10000)
        assert(th.getDropRate(0, 0) == 0)
        assert(th.getDropRate(14, 0) == 0)
    end)

    it('remaps by tier and rarity bracket', function()
        -- TH0 VC base 2400 → 2400
        assert(th.getDropRate(0, 2400) == 2400)
        -- TH1 VC → 4800
        assert(th.getDropRate(1, 2400) == 4800)
        -- TH0 UR base 10 → 10 (bracket 7: rate 10 >= 0)
        assert(th.getDropRate(0, 10) == 10)
        -- TH14 UR → 150
        assert(th.getDropRate(14, 10) == 150)
        -- TH5 VC → 6666
        assert(th.getDropRate(5, 3000) == 6666)
        -- TH8 rare base 500 → 1050
        assert(th.getDropRate(8, 500) == 1050)
        -- out-of-range tier clamps to 14
        assert(th.getDropRate(99, 2400) == 8000)
        -- out-of-range rate clamps to 10000 short-circuit
        assert(th.getDropRate(0, 20000) == 10000)
    end)
end)
