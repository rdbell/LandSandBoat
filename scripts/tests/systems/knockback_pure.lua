-----------------------------------
-- Pure system tests for knockback dual-wire helpers (slice 6693).
-- Calls production xi.combat.knockback pure exports.
-- Goldens match internal/knockback (0911 / 1581).
-----------------------------------

require('scripts/globals/combat/knockback')

local kb = xi.combat.knockback
local levels = xi.action.knockback

describe('knockback constants', function()
    it('pins NONE and LEVEL7 bounds', function()
        assert(kb.levelNone == levels.NONE)
        assert(kb.level7 == levels.LEVEL7)
        assert(kb.levelNone == 0)
        assert(kb.level7 == 7)
    end)
end)

describe('calculateFromParams', function()
    it('passes through in-range skill with zero reduction', function()
        for skill = 0, 7 do
            assert(kb.calculateFromParams({ skillKnockback = skill, knockbackReductionMod = 0 }) == skill)
        end
    end)

    it('subtracts reduction and clamps floor/ceiling', function()
        -- Aqua Blast LEVEL5 with mod 2 → LEVEL3
        assert(kb.calculateFromParams({ skillKnockback = 5, knockbackReductionMod = 2 }) == 3)
        assert(kb.calculateFromParams({ skillKnockback = 5, knockbackReductionMod = 1 }) == 4)
        assert(kb.calculateFromParams({ skillKnockback = 7, knockbackReductionMod = 3 }) == 4)

        -- Floor NONE
        assert(kb.calculateFromParams({ skillKnockback = 5, knockbackReductionMod = 5 }) == 0)
        assert(kb.calculateFromParams({ skillKnockback = 5, knockbackReductionMod = 8 }) == 0)
        assert(kb.calculateFromParams({ skillKnockback = 7, knockbackReductionMod = 100 }) == 0)

        -- Ceiling LEVEL7
        assert(kb.calculateFromParams({ skillKnockback = 8, knockbackReductionMod = 0 }) == 7)
        assert(kb.calculateFromParams({ skillKnockback = 100, knockbackReductionMod = 0 }) == 7)
        assert(kb.calculateFromParams({ skillKnockback = 10, knockbackReductionMod = 1 }) == 7)
        assert(kb.calculateFromParams({ skillKnockback = 10, knockbackReductionMod = 3 }) == 7)
    end)

    it('handles negative skill and negative reduction', function()
        assert(kb.calculateFromParams({ skillKnockback = -1, knockbackReductionMod = 0 }) == 0)
        assert(kb.calculateFromParams({ skillKnockback = -5, knockbackReductionMod = 2 }) == 0)
        -- Negative reduction increases knockback
        assert(kb.calculateFromParams({ skillKnockback = 3, knockbackReductionMod = -2 }) == 5)
        assert(kb.calculateFromParams({ skillKnockback = 7, knockbackReductionMod = -1 }) == 7)
        assert(kb.calculateFromParams({ skillKnockback = 0, knockbackReductionMod = -7 }) == 7)
        assert(kb.calculateFromParams({ skillKnockback = 0, knockbackReductionMod = -8 }) == 7)
    end)

    it('steps down each level with unit reduction', function()
        local want = { 0, 0, 1, 2, 3, 4, 5, 6 } -- index skill+1
        for skill = 0, 7 do
            assert(kb.calculateFromParams({
                skillKnockback = skill, knockbackReductionMod = 1,
            }) == want[skill + 1])
        end
    end)
end)
