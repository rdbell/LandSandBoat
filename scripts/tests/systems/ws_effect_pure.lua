-----------------------------------
-- Pure system tests for handleWeaponskillEffect dual-wire (slice 6755).
-- Calls production xi.weaponskills.shouldApplyWeaponskillEffectFromParams.
-- Goldens match internal/wsformula.ShouldApplyWeaponskillEffect (1053).
-----------------------------------

require('scripts/globals/weaponskills')

local w = xi.weaponskills

describe('WS effect apply pure gate', function()
    it('applies when damage positive and all injects clear', function()
        assert(w.shouldApplyWeaponskillEffectFromParams({ damage = 1 }))
        assert(w.shouldApplyWeaponskillEffectFromParams({ damage = 9999 }))
    end)

    it('blocks non-positive damage', function()
        assert(not w.shouldApplyWeaponskillEffectFromParams({ damage = 0 }))
        assert(not w.shouldApplyWeaponskillEffectFromParams({ damage = -1 }))
        assert(not w.shouldApplyWeaponskillEffectFromParams({}))
    end)

    it('blocks each status inject independently', function()
        assert(not w.shouldApplyWeaponskillEffectFromParams({
            damage = 10, hasStatusEffect = true,
        }))
        assert(not w.shouldApplyWeaponskillEffectFromParams({
            damage = 10, isTargetImmune = true,
        }))
        assert(not w.shouldApplyWeaponskillEffectFromParams({
            damage = 10, isTargetResistant = true,
        }))
        assert(not w.shouldApplyWeaponskillEffectFromParams({
            damage = 10, isEffectNullified = true,
        }))
    end)

    it('blocks multi-gate combinations', function()
        assert(not w.shouldApplyWeaponskillEffectFromParams({
            damage = 0,
            hasStatusEffect = true,
            isTargetImmune = true,
            isTargetResistant = true,
            isEffectNullified = true,
        }))
        assert(not w.shouldApplyWeaponskillEffectFromParams({
            damage = 10, hasStatusEffect = true, isTargetImmune = true,
        }))
    end)
end)
