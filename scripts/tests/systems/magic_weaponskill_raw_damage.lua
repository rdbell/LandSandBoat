require('scripts/globals/weaponskills')

describe('Magic weaponskill raw damage', function()
    it('multiplies base stats by combined fTP', function()
        -- (0 + 75 + 2 + 8) * (1.5 + 0) = 85 * 1.5 = 127.5
        assert(xi.weaponskills.magicWeaponskillRawDamage(0, 75, 8, 1.5, 0) == 127.5)
        -- with WSC and gear fTP: (20 + 75 + 2 + 0) * (1 + 0.25) = 97 * 1.25 = 121.25
        assert(xi.weaponskills.magicWeaponskillRawDamage(20, 75, 0, 1, 0.25) == 121.25)
    end)

    it('includes the flat main-level plus two base', function()
        assert(xi.weaponskills.magicWeaponskillRawDamage(0, 1, 0, 1, 0) == 3)
        assert(xi.weaponskills.magicWeaponskillRawDamage(0, 0, 0, 1, 0) == 2)
    end)
end)

describe('Magic weaponskill WSD product', function()
    it('applies all-hits WSD as a percent', function()
        -- 100 * 1.2 = 120
        assert(xi.weaponskills.magicWeaponskillWSDProduct(100, 20, 0, false, 0) == 120)
    end)

    it('stacks per-WS WSD only when positive and not a pet', function()
        -- 10 + 15 = 25% → 125
        assert(xi.weaponskills.magicWeaponskillWSDProduct(100, 10, 15, false, 0) == 125)
        -- pet: per-WS ignored
        assert(xi.weaponskills.magicWeaponskillWSDProduct(100, 10, 15, true, 0) == 110)
        -- non-positive per-WS ignored
        assert(xi.weaponskills.magicWeaponskillWSDProduct(100, 10, 0, false, 0) == 110)
        assert(xi.weaponskills.magicWeaponskillWSDProduct(100, 10, -5, false, 0) == 110)
    end)

    it('applies first-hit WSD after all-hits', function()
        -- all-hits 20% → 120, first-hit 10% → 120 + 12 = 132
        assert(xi.weaponskills.magicWeaponskillWSDProduct(100, 20, 0, false, 10) == 132)
    end)
end)
