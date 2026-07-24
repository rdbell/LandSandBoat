require('scripts/globals/mobskills')

describe('Mob skill magic burst rank bonus', function()
    it('uses the extremes outside the mid table', function()
        assert(xi.mobskills.magicBurstRankBonus(-3) == 1.5)
        assert(xi.mobskills.magicBurstRankBonus(-4) == 1.5)
        assert(xi.mobskills.magicBurstRankBonus(5) == 0)
        assert(xi.mobskills.magicBurstRankBonus(6) == 0)
    end)

    it('indexes the mid table for ranks -2 through 4', function()
        assert(xi.mobskills.magicBurstRankBonus(-2) == 1.15)
        assert(xi.mobskills.magicBurstRankBonus(-1) == 0.85)
        assert(xi.mobskills.magicBurstRankBonus(0) == 0.6)
        assert(xi.mobskills.magicBurstRankBonus(1) == 0.5)
        assert(xi.mobskills.magicBurstRankBonus(2) == 0.4)
        assert(xi.mobskills.magicBurstRankBonus(3) == 0.15)
        assert(xi.mobskills.magicBurstRankBonus(4) == 0.05)
    end)
end)

describe('Mob skill magic burst multiplier', function()
    it('returns one when the element is NONE', function()
        -- Even with a live skillchain, no element means no burst.
        assert(xi.mobskills.mobMagicBurstMultiplier(xi.element.NONE, -3, 3) == 1.0)
    end)

    it('returns one when there is no skillchain', function()
        -- Rank is ignored until SC >= 1.
        assert(xi.mobskills.mobMagicBurstMultiplier(xi.element.FIRE, -3, 0) == 1.0)
    end)

    it('adds the tier base and rank bonus when SC is active', function()
        -- SC1 tier 1.3 + rank -2 (1.15) = 2.45
        assert(xi.mobskills.mobMagicBurstMultiplier(xi.element.FIRE, -2, 1) == 2.45)
        -- SC5 tier 1.5 + rank 0 (0.6) = 2.1
        assert(xi.mobskills.mobMagicBurstMultiplier(xi.element.FIRE, 0, 5) == 2.1)
        -- SC2 tier 1.35 + rank >=5 (0) = 1.35
        assert(xi.mobskills.mobMagicBurstMultiplier(xi.element.FIRE, 5, 2) == 1.35)
    end)

    it('uses the published tier table', function()
        assert(xi.mobskills.burstMultipliersByTier[1] == 1.3)
        assert(xi.mobskills.burstMultipliersByTier[2] == 1.35)
        assert(xi.mobskills.burstMultipliersByTier[3] == 1.40)
        assert(xi.mobskills.burstMultipliersByTier[4] == 1.45)
        assert(xi.mobskills.burstMultipliersByTier[5] == 1.5)
    end)

    -- Spell MB uses 1.25 + rank + SC/10; pin that mobs do not.
    it('does not use the spell magic-burst formula', function()
        -- Spell would be 1.25 + 0.6 + 0.1 = 1.95 for rank0 SC1.
        -- Mob is 1.3 + 0.6 = 1.9.
        assert(xi.mobskills.mobMagicBurstMultiplier(xi.element.FIRE, 0, 1) == 1.9)
    end)
end)
