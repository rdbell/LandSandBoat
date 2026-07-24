require('scripts/globals/weaponskills')

describe('Hybrid weaponskill magic base', function()
    it('floors physical times fTP plus magic damage mod', function()
        -- floor(100 * 1.5 + 10) = 160
        assert(xi.weaponskills.hybridWeaponskillMagicBase(100, 1.5, 10, 0, 0) == 160)
        -- floor(100 * 1.5 + 0) = 150
        assert(xi.weaponskills.hybridWeaponskillMagicBase(100, 1.5, 0, 0, 0) == 150)
    end)

    it('applies ALL_WSDMG as a percent after the base floor', function()
        -- base 100, +20% WSD → floor(100 * 120 / 100) = 120
        assert(xi.weaponskills.hybridWeaponskillMagicBase(100, 1, 0, 20, 0) == 120)
    end)

    it('stacks per-WS WSD only when strictly positive', function()
        -- all 10 + per 15 → 25% → floor(100 * 125 / 100) = 125
        assert(xi.weaponskills.hybridWeaponskillMagicBase(100, 1, 0, 10, 15) == 125)
        -- per-WS zero does not add
        assert(xi.weaponskills.hybridWeaponskillMagicBase(100, 1, 0, 10, 0) == 110)
        -- per-WS negative is ignored (only > 0 stacks)
        assert(xi.weaponskills.hybridWeaponskillMagicBase(100, 1, 0, 10, -5) == 110)
    end)

    it('floors after the WSD product', function()
        -- base floor(101 * 1) = 101, * 1.1 = 111.1 → floor 111
        assert(xi.weaponskills.hybridWeaponskillMagicBase(101, 1, 0, 10, 0) == 111)
    end)
end)

describe('Hybrid weaponskill magic bonus fTP', function()
    -- Applied after ability bonuses in the live path; pure floor add only.
    it('adds bonus fTP times physical', function()
        assert(xi.weaponskills.hybridWeaponskillMagicBonusFTP(100, 100, 0.5) == 150)
        assert(xi.weaponskills.hybridWeaponskillMagicBonusFTP(120, 100, 0.5) == 170)
    end)

    it('floors the sum', function()
        -- 100 + 0.5 * 101 = 150.5 → 150
        assert(xi.weaponskills.hybridWeaponskillMagicBonusFTP(100, 101, 0.5) == 150)
    end)
end)
