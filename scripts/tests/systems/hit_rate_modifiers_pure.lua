-----------------------------------
-- Pure system tests for hit-rate modifiers dual-wire (slice 6683).
-- Calls production xi.combat.physicalHitRate.hitRateModifiersFromParams.
-- Goldens match internal/hitratemod.HitRateModifiers (0918).
-----------------------------------

require('scripts/globals/combat/physical_hit_rate')

local phr = xi.combat.physicalHitRate

local function mods(params)
    return phr.hitRateModifiersFromParams(params)
end

describe('Hit rate modifier constants', function()
    it('pins geometry and flourish scales', function()
        assert(phr.behindAngle == 23)
        assert(phr.facingConeYonin == 64)
        assert(phr.buildingFlourishBase == 40)
        assert(phr.buildingFlourishSubPowerScale == 2)
        assert(phr.yoninJPScale == 2)
    end)
end)

describe('Hit rate modifiers empty', function()
    it('returns zero bonuses', function()
        local acc, eva = mods({})
        assert(acc == 0 and eva == 0)
    end)
end)

describe('Hit rate modifiers Building Flourish', function()
    it('applies only on melee weaponskill with power >= 1', function()
        local acc, eva = mods({
            isWeaponskill = true,
            hasBuildingFlourish = true,
            buildingFlourishPower = 1,
            buildingFlourishSubPower = 0,
        })
        assert(acc == 40 and eva == 0)

        acc, eva = mods({
            isWeaponskill = true,
            hasBuildingFlourish = true,
            buildingFlourishPower = 2,
            buildingFlourishSubPower = 5,
        })
        assert(acc == 50 and eva == 0)

        acc = mods({
            isWeaponskill = true,
            hasBuildingFlourish = true,
            buildingFlourishPower = 0,
            buildingFlourishSubPower = 10,
        })
        assert(acc == 0)

        acc = mods({
            isWeaponskill = false,
            hasBuildingFlourish = true,
            buildingFlourishPower = 2,
            buildingFlourishSubPower = 5,
        })
        assert(acc == 0)

        acc = mods({
            isRanged = true,
            isWeaponskill = true,
            hasBuildingFlourish = true,
            buildingFlourishPower = 2,
            buildingFlourishSubPower = 5,
        })
        assert(acc == 0)
    end)
end)

describe('Hit rate modifiers Innin', function()
    it('adds power+JP only when behind on melee', function()
        local acc, eva = mods({
            hasInnin = true,
            inninPower = 30,
            inninJP = 5,
            isBehind23 = true,
        })
        assert(acc == 35 and eva == 0)

        acc = mods({
            hasInnin = true,
            inninPower = 30,
            inninJP = 5,
            isBehind23 = false,
        })
        assert(acc == 0)

        acc = mods({
            isRanged = true,
            hasInnin = true,
            inninPower = 30,
            inninJP = 5,
            isBehind23 = true,
        })
        assert(acc == 0)
    end)
end)

describe('Hit rate modifiers Closed Position', function()
    it('attacker PC facing on melee; target PC facing on any', function()
        local acc, eva = mods({
            attackerIsPC = true,
            attackerIsFacing = true,
            attackerClosedPositionMerit = 12,
        })
        assert(acc == 12 and eva == 0)

        acc = mods({
            attackerIsPC = true,
            attackerIsFacing = false,
            attackerClosedPositionMerit = 12,
        })
        assert(acc == 0)

        acc = mods({
            isRanged = true,
            attackerIsPC = true,
            attackerIsFacing = true,
            attackerClosedPositionMerit = 12,
        })
        assert(acc == 0)

        acc, eva = mods({
            targetIsPC = true,
            targetIsFacing = true,
            targetClosedPositionMerit = 8,
        })
        assert(acc == 0 and eva == 8)
    end)
end)

describe('Hit rate modifiers Ambush', function()
    it('applies on melee and ranged when behind with trait', function()
        local acc = mods({
            hasAmbushTrait = true,
            ambushMerit = 15,
            isBehind23 = true,
        })
        assert(acc == 15)

        acc = mods({
            hasAmbushTrait = true,
            ambushMerit = 15,
            isBehind23 = false,
        })
        assert(acc == 0)

        acc = mods({
            isRanged = true,
            hasAmbushTrait = true,
            ambushMerit = 15,
            isBehind23 = true,
        })
        assert(acc == 15)
    end)
end)

describe('Hit rate modifiers Yonin', function()
    it('adds power+2*JP when facing cone 64', function()
        local acc, eva = mods({
            hasYonin = true,
            yoninPower = 20,
            yoninJP = 4,
            isFacing64 = true,
        })
        assert(acc == 0 and eva == 28)

        acc, eva = mods({
            hasYonin = true,
            yoninPower = 20,
            yoninJP = 4,
            isFacing64 = false,
        })
        assert(eva == 0)

        -- ranged still applies
        acc, eva = mods({
            isRanged = true,
            hasYonin = true,
            yoninPower = 10,
            yoninJP = 1,
            isFacing64 = true,
        })
        assert(eva == 12)
    end)
end)

describe('Hit rate modifiers Flash penalty', function()
    it('subtracts from accBonus', function()
        local acc = mods({
            attackerIsPC = true,
            attackerIsFacing = true,
            attackerClosedPositionMerit = 20,
            flashPenalty = 5,
        })
        assert(acc == 15)

        acc = mods({ flashPenalty = 10 })
        assert(acc == -10)
    end)
end)

describe('Hit rate modifiers combinations', function()
    it('stacks melee innin+ambush+flourish and yonin+target closed', function()
        local acc, eva = mods({
            isWeaponskill = true,
            hasBuildingFlourish = true,
            buildingFlourishPower = 1,
            buildingFlourishSubPower = 0, -- +40
            hasInnin = true,
            inninPower = 10,
            inninJP = 2, -- +12
            isBehind23 = true,
            hasAmbushTrait = true,
            ambushMerit = 5, -- +5
            hasYonin = true,
            yoninPower = 10,
            yoninJP = 3, -- +16
            isFacing64 = true,
            targetIsPC = true,
            targetIsFacing = true,
            targetClosedPositionMerit = 4, -- +4 eva
            flashPenalty = 3,
        })
        -- acc: 40+12+5-3 = 54
        assert(acc == 54)
        -- eva: 16+4 = 20
        assert(eva == 20)
    end)
end)
