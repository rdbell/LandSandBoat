require('scripts/globals/automaton')

describe('Automaton extra hits', function()
    it('adds a hit for every roll at or under the rate', function()
        assert(xi.automaton.extraHitsFromRolls(50, 3, { 1, 50, 51 }) == 2)
    end)

    it('rolls once per swing', function()
        assert(xi.automaton.extraHitsFromRolls(100, 4, { 1, 1, 1, 1 }) == 4)
        assert(xi.automaton.extraHitsFromRolls(100, 0, {}) == 0)
    end)

    it('skips rolling entirely without Double Attack', function()
        assert(xi.automaton.extraHitsFromRolls(0, 3, { 1, 1, 1 }) == 0)
    end)

    it('clamps the rate to 0..100', function()
        assert(xi.automaton.extraHitsFromRolls(150, 2, { 100, 100 }) == 2)
        assert(xi.automaton.extraHitsFromRolls(-50, 2, { 1, 1 }) == 0)
    end)
end)

describe('Automaton Equalizer', function()
    it('returns damage untouched without the attachment', function()
        assert(xi.automaton.equalizerDamage(500, 0, 1000) == 500)
    end)

    it('returns non-positive damage untouched', function()
        assert(xi.automaton.equalizerDamage(0, 100, 1000) == 0)
        assert(xi.automaton.equalizerDamage(-5, 100, 1000) == -5)
    end)

    it('reduces more sharply as damage approaches max HP', function()
        -- 100/1000 * 100/100 = 0.10 reduction
        assert(xi.automaton.equalizerDamage(100, 100, 1000) == 90)
        -- 500/1000 * 100/100 = 0.50 reduction
        assert(xi.automaton.equalizerDamage(500, 100, 1000) == 250)
    end)

    it('truncates the reduction rate to two decimals', function()
        -- 111/1000 * 100/100 = 0.111 -> truncated to 0.11
        assert(xi.automaton.equalizerDamage(111, 100, 1000) == math.floor(111 * 0.89))
    end)

    -- At the cap, 1 - 0.9 is 0.09999999999999998 in binary floating point, so
    -- the product lands just under 95 and floors to 94. Pinned as a literal
    -- because the naive arithmetic (950 * 0.10) gives 95 instead.
    it('caps the reduction at ninety percent', function()
        assert(xi.automaton.equalizerDamage(950, 100, 1000) == 94)
    end)
end)

describe('Automaton Flame Holder', function()
    it('scales every fTP tier', function()
        local ftp = xi.automaton.flameHolderFTP({ 1.0, 2.0, 3.0 }, 200)
        assert(ftp[1] == 2.0 and ftp[2] == 4.0 and ftp[3] == 6.0)
    end)

    it('leaves fTP untouched without the modifier', function()
        local ftp = xi.automaton.flameHolderFTP({ 1.0, 2.0, 3.0 }, 0)
        assert(ftp[1] == 1.0 and ftp[2] == 2.0 and ftp[3] == 3.0)
    end)
end)

describe('Automaton ranged base damage', function()
    it('scales ranged damage by the attachment percentage', function()
        assert(xi.automaton.rangedBaseDamage(100, 0) == 100)
        assert(xi.automaton.rangedBaseDamage(100, 50) == 150)
    end)
end)

describe('Automaton Volt Gun potency', function()
    it('derives base potency from the best skill level', function()
        -- floor(160/16) = 10, dINT 0 -> power 0
        assert(xi.automaton.voltGunPotency(0, 160, 0) == 10)
    end)

    it('adds half the INT delta', function()
        assert(xi.automaton.voltGunPotency(0, 160, 20) == 20)
    end)

    it('clamps the INT contribution to -10..30', function()
        assert(xi.automaton.voltGunPotency(0, 160, 1000) == 40)
        assert(xi.automaton.voltGunPotency(0, 320, -1000) == 10)
    end)

    it('scales by the attachment modifier', function()
        assert(xi.automaton.voltGunPotency(100, 160, 0) == 20)
    end)

    it('never drops below one', function()
        assert(xi.automaton.voltGunPotency(0, 0, -1000) == 1)
    end)
end)

describe('Automaton Volt Gun negative INT delta', function()
    -- math.floor(-5/2) is -3, not the -2 a truncating division would give.
    it('floors the INT contribution rather than truncating', function()
        assert(xi.automaton.voltGunPotency(0, 160, -5) == 7)
    end)
end)
