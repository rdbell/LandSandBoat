-----------------------------------
-- Pure system tests for ability AoE dual-wire helpers (slice 6695).
-- Calls production xi.combat.abilityAoE pure exports.
-- Goldens match internal/abilityaoe (0846 / 1581).
-----------------------------------

require('scripts/globals/combat/ability_aoe')

local aoe = xi.combat.abilityAoE

describe('ability AoE constants', function()
    it('pins override radius and enum ids used by overrides', function()
        assert(aoe.overrideRadius == 10)
        assert(xi.jobAbility.LIEMENT == 373)
        assert(xi.jobAbility.HEALING_WALTZ == 194)
        assert(xi.recastID.PHANTOM_ROLL == 193)
        assert(xi.recastID.DOUBLE_UP == 194)
        assert(xi.aoeType.NONE == 0)
        assert(xi.aoeType.ROUND == 1)
    end)
end)

describe('calculateTypeAndRadiusFromParams', function()
    it('passes through base type and radius', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = 1, baseType = xi.aoeType.CONE, baseRadius = 7,
        })
        assert(got[1] == xi.aoeType.CONE and got[2] == 7)

        got = aoe.calculateTypeAndRadiusFromParams({
            baseType = xi.aoeType.NONE, baseRadius = 0,
        })
        assert(got[1] == xi.aoeType.NONE and got[2] == 0)
    end)

    it('overrides Liement with Epeolatry to ROUND 10', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.LIEMENT, baseType = xi.aoeType.NONE, baseRadius = 0,
            liementExtendsToArea = 1,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.LIEMENT, baseType = xi.aoeType.NONE, baseRadius = 0,
            liementExtendsToArea = 0,
        })
        assert(got[1] == xi.aoeType.NONE and got[2] == 0)

        got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = 1, baseType = xi.aoeType.CONE, baseRadius = 3, liementExtendsToArea = 99,
        })
        assert(got[1] == xi.aoeType.CONE and got[2] == 3)
    end)

    it('overrides Healing Waltz with Contradance to ROUND 10', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.HEALING_WALTZ, hasContradance = true,
            baseType = xi.aoeType.NONE, baseRadius = 0,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.HEALING_WALTZ, hasContradance = false,
            baseType = xi.aoeType.NONE, baseRadius = 0,
        })
        assert(got[1] == xi.aoeType.NONE and got[2] == 0)
    end)

    it('forces ROUND and adds ROLL_RANGE for COR rolls', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            recastID = xi.recastID.PHANTOM_ROLL, baseType = xi.aoeType.NONE,
            baseRadius = 8, rollRange = 8,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 16)

        got = aoe.calculateTypeAndRadiusFromParams({
            recastID = xi.recastID.DOUBLE_UP, baseType = xi.aoeType.CONE,
            baseRadius = 8, rollRange = 0,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 8)

        got = aoe.calculateTypeAndRadiusFromParams({
            recastID = xi.recastID.PHANTOM_ROLL, baseRadius = 8, rollRange = -2,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 6)
    end)

    it('prioritizes Liement/Contradance over roll recast', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.LIEMENT, recastID = xi.recastID.PHANTOM_ROLL,
            baseRadius = 8, liementExtendsToArea = 1, rollRange = 8,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.HEALING_WALTZ, recastID = xi.recastID.DOUBLE_UP,
            baseRadius = 8, hasContradance = true, rollRange = 8,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 10)

        -- Waltz without Contradance falls through to roll path
        got = aoe.calculateTypeAndRadiusFromParams({
            abilityID = xi.jobAbility.HEALING_WALTZ, recastID = xi.recastID.PHANTOM_ROLL,
            baseRadius = 8, hasContradance = false, rollRange = 8,
        })
        assert(got[1] == xi.aoeType.ROUND and got[2] == 16)
    end)
end)
