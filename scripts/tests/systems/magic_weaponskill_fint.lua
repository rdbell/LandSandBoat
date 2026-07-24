require('scripts/globals/weaponskills')

describe('Magic weaponskill fINT', function()
    it('doubles the stat delta for ranged weaponskills', function()
        assert(xi.weaponskills.magicWeaponskillFint(true, false, 10) == 20)
        assert(xi.weaponskills.magicWeaponskillFint(true, false, -10) == -20)
        assert(xi.weaponskills.magicWeaponskillFint(true, false, 0) == 0)
    end)

    it('clamps ranged to plus or minus thirty-two', function()
        assert(xi.weaponskills.magicWeaponskillFint(true, false, 1000) == 32)
        assert(xi.weaponskills.magicWeaponskillFint(true, false, -1000) == -32)
    end)

    -- CHR weaponskills clamp without flooring, so the result can be fractional.
    it('scales CHR by one and a half without flooring', function()
        assert(xi.weaponskills.magicWeaponskillFint(false, true, 3) == 4.5)
        assert(xi.weaponskills.magicWeaponskillFint(false, true, 10) == 15)
    end)

    it('clamps CHR to a far wider bound', function()
        assert(xi.weaponskills.magicWeaponskillFint(false, true, 10000) == 651)
        assert(xi.weaponskills.magicWeaponskillFint(false, true, -10000) == -651)
    end)

    it('adds eight and halves the delta by default', function()
        assert(xi.weaponskills.magicWeaponskillFint(false, false, 0) == 8)
        assert(xi.weaponskills.magicWeaponskillFint(false, false, 10) == 13)
    end)

    -- The default branch floors after clamping, so an odd delta rounds down.
    it('floors the default branch', function()
        assert(xi.weaponskills.magicWeaponskillFint(false, false, 1) == 8)
        assert(xi.weaponskills.magicWeaponskillFint(false, false, -1) == 7)
    end)

    it('clamps the default branch before flooring', function()
        assert(xi.weaponskills.magicWeaponskillFint(false, false, 1000) == 32)
        assert(xi.weaponskills.magicWeaponskillFint(false, false, -1000) == -32)
    end)

    -- Ranged takes precedence over the CHR branch.
    it('prefers the ranged branch when both apply', function()
        assert(xi.weaponskills.magicWeaponskillFint(true, true, 10) == 20)
    end)
end)
