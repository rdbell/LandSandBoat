-----------------------------------
-- Pure system tests for pDIF attack-assembly dual-wire (slice 6759).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/pdif BuildingFlourishBonus / MeleeActorAttack /
-- RangedActorAttack / BaseRatio / MeleeWRatio / MeleeRandomFactor.
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('buildingFlourishBonusFromParams', function()
    it('requires effect power >= 2', function()
        assert(p.buildingFlourishBonusFromParams({ hasEffect = false, power = 3, meritCount = 5 }) == 1)
        assert(p.buildingFlourishBonusFromParams({ hasEffect = true, power = 1, meritCount = 5 }) == 1)
        assert(almost(p.buildingFlourishBonusFromParams({
            hasEffect = true, power = 2, meritCount = 0,
        }), 1.25))
        assert(almost(p.buildingFlourishBonusFromParams({
            hasEffect = true, power = 2, meritCount = 5,
        }), 1.30))
        assert(p.buildingFlourishMinPower == 2)
        assert(almost(p.buildingFlourishBaseBonus, 1.25))
        assert(almost(p.buildingFlourishMeritStep, 0.01))
    end)
end)

describe('actor attack products', function()
    it('melee floors ATT * wsMod * flourish with min 1', function()
        assert(p.meleeActorAttackFromParams({
            att = 100, wsAttackMod = 1, flourishBonus = 1.25,
        }) == 125)
        assert(p.meleeActorAttackFromParams({
            att = 10, wsAttackMod = 1.1, flourishBonus = 1,
        }) == 11)
        assert(p.meleeActorAttackFromParams({
            att = 0.4, wsAttackMod = 1, flourishBonus = 1,
        }) == 1)
    end)

    it('ranged floors (RATT + bonus - distance) * mods with min 1', function()
        assert(p.rangedActorAttackFromParams({
            ratt = 100, bonusRangedAttack = 10, distancePenalty = 5,
            wsAttackMod = 1, flourishBonus = 1,
        }) == 105)
        assert(p.rangedActorAttackFromParams({
            ratt = 50, bonusRangedAttack = 0, distancePenalty = 0,
            wsAttackMod = 1.25, flourishBonus = 1.25,
        }) == 78)
    end)
end)

describe('baseRatio wRatio and melee random', function()
    it('baseRatio and melee wRatio', function()
        assert(p.baseRatioFromParams({ actorAttack = 100, targetDefense = 50 }) == 2)
        assert(p.baseRatioFromParams({ actorAttack = 100, targetDefense = 0 }) == 0)
        assert(p.meleeWRatioFromParams({ baseRatio = 1.5, isCritical = false }) == 1.5)
        assert(p.meleeWRatioFromParams({ baseRatio = 1.5, isCritical = true }) == 2.5)
    end)

    it('melee random factor clamps step 0..5', function()
        assert(p.meleeRandomFactorFromParams({ step = 0 }) == 1)
        assert(almost(p.meleeRandomFactorFromParams({ step = 5 }), 1.05))
        assert(almost(p.meleeRandomFactorFromParams({ step = 9 }), 1.05))
        assert(p.meleeRandomFactorFromParams({ step = -3 }) == 1)
    end)
end)
