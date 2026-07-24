require('scripts/globals/mobskills')

describe('Mob skill hybrid magic damage', function()
    -- Neutral multipliers: only the fixed 0.5 hybrid scale remains.
    it('halves physical damage with neutral multipliers', function()
        assert(xi.mobskills.hybridMagicDamage(100, 1, 1, 1, 1, 1, 1, 1) == 50)
        assert(xi.mobskills.hybridMagicDamage(101, 1, 1, 1, 1, 1, 1, 1) == 50)
    end)

    it('floors physical damage before scaling', function()
        assert(xi.mobskills.hybridMagicDamage(100.9, 1, 1, 1, 1, 1, 1, 1) == 50)
    end)

    it('applies each multiplier with an intermediate floor', function()
        -- 100 * 1.5 = 150, * 0.5 resist = 75, * 0.5 hybrid = 37
        assert(xi.mobskills.hybridMagicDamage(100, 1.5, 0.5, 1, 1, 1, 1, 1) == 37)
    end)

    it('applies day/weather and MAB even when absorb is non-positive', function()
        -- absorb 0 forces sdt/resist/adj to 1, then * day * mab * 0 * null * 0.5
        assert(xi.mobskills.hybridMagicDamage(100, 2, 2, 1.5, 1.2, 2, 0, 1) == 0)
        -- absorb -1 (heal path): 100 * 1 * 1 * 1.5 * 1.2 * 1 * -1 * 1 * 0.5
        -- floors: 100, 100, 150, 180, 180, -180, -180, -90
        assert(xi.mobskills.hybridMagicDamage(100, 2, 2, 1.5, 1.2, 2, -1, 1) == -90)
    end)

    -- When absorb is non-positive, injected SDT/resist/adj must not apply.
    it('ignores SDT resist and adjustment when absorb is not positive', function()
        assert(xi.mobskills.hybridMagicDamage(100, 2, 0.25, 1, 1, 0.5, 0, 1) == 0)
        -- absorb -1 with bogus sdt/resist/adj that would otherwise zero the product
        assert(xi.mobskills.hybridMagicDamage(100, 0, 0, 1, 1, 0, -1, 1) == -50)
    end)

    it('keeps SDT resist and adjustment when absorb is positive', function()
        -- 100 * 0.5 sdt * 1 * 1 * 1 * 1 * 1 * 1 * 0.5 = 25
        assert(xi.mobskills.hybridMagicDamage(100, 0.5, 1, 1, 1, 1, 1, 1) == 25)
    end)

    it('applies nullification after absorb', function()
        assert(xi.mobskills.hybridMagicDamage(100, 1, 1, 1, 1, 1, 1, 0) == 0)
    end)
end)
