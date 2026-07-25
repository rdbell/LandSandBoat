-----------------------------------
-- Pure system tests for chooseAction dual-wire helpers (slice 6701).
-- Calls production xi.combat.behavior pure exports.
-- Goldens match internal/combatbehavior (1037).
-----------------------------------

require('scripts/globals/combat/entity_behavior')

local beh = xi.combat.behavior

describe('chooseAction constants', function()
    it('pins distance, default weight, and action types', function()
        assert(beh.allyMaxDistance == 8)
        assert(beh.defaultWeight == 100)
        assert(xi.action.type.NONE == 0)
        assert(xi.action.type.DAMAGE_TARGET == 1)
        assert(xi.action.type.DRAIN_MP == 11)
        assert(xi.effect.SILENCE == 6)
        assert(xi.effect.BURN == 128)
        assert(xi.effect.DROWN == 133)
    end)
end)

describe('isElementalDoT and normalizeWeight', function()
    it('recognizes Burn..Drown family only', function()
        assert(beh.isElementalDoT(xi.effect.BURN))
        assert(beh.isElementalDoT(xi.effect.FROST))
        assert(beh.isElementalDoT(xi.effect.CHOKE))
        assert(beh.isElementalDoT(xi.effect.RASP))
        assert(beh.isElementalDoT(xi.effect.SHOCK))
        assert(beh.isElementalDoT(xi.effect.DROWN))
        assert(not beh.isElementalDoT(xi.effect.SILENCE))
        assert(not beh.isElementalDoT(0))
        assert(not beh.isElementalDoT(127))
        assert(not beh.isElementalDoT(134))
    end)

    it('defaults missing weight to 100; keeps explicit 0', function()
        assert(beh.normalizeWeight(0, false) == 100)
        assert(beh.normalizeWeight(0, true) == 0)
        assert(beh.normalizeWeight(25, true) == 25)
        assert(beh.normalizeWeight(nil, false) == 100)
    end)
end)

describe('enable/disable action flags', function()
    it('pins all-false and all-true triples', function()
        local d = beh.disableAllActionsFlags()
        assert(d.autoAttack == false and d.magicCasting == false and d.mobAbility == false)
        local e = beh.enableAllActionsFlags()
        assert(e.autoAttack == true and e.magicCasting == true and e.mobAbility == true)
    end)
end)

describe('totalWeight and pickWeightedFromParams', function()
    it('sums weights and picks by cumulative roll', function()
        local list = {
            { 1, 'a', 10 },
            { 2, 'b', 20 },
            { 3, 'c', 70 },
        }
        assert(beh.totalWeight(list) == 100)

        local id, tgt, ok = beh.pickWeightedFromParams(list, 1)
        assert(ok and id == 1 and tgt == 'a')
        id, tgt, ok = beh.pickWeightedFromParams(list, 10)
        assert(ok and id == 1)
        id, tgt, ok = beh.pickWeightedFromParams(list, 11)
        assert(ok and id == 2 and tgt == 'b')
        id, tgt, ok = beh.pickWeightedFromParams(list, 30)
        assert(ok and id == 2)
        id, tgt, ok = beh.pickWeightedFromParams(list, 31)
        assert(ok and id == 3 and tgt == 'c')
        id, tgt, ok = beh.pickWeightedFromParams(list, 100)
        assert(ok and id == 3)

        id, tgt, ok = beh.pickWeightedFromParams({}, 1)
        assert(not ok and id == 0 and tgt == nil)
        id, tgt, ok = beh.pickWeightedFromParams({ { 1, 'a', 0 } }, 1)
        assert(not ok)
    end)
end)
