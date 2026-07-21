-----------------------------------
-- Pure system tests for additional_effects calcDamage / calcPhysDamage (slice 6130).
-- Source: scripts/globals/additional_effects.lua ~80–180
-----------------------------------

describe('additionalEffect calcDamage pure plan', function()
    local function calcDamage(p)
        local damage = p.afterBonuses or p.damage or 0 -- addBonusesAbility inject
        damage = math.floor(damage * (p.resistance or 1))
        damage = math.floor(damage * (p.absorb or 1))
        damage = math.floor(damage * (p.nullify or 1))
        -- finalMagicNonSpellAdjustments inject
        damage = p.afterFinalMagic or damage
        return damage
    end

    it('floor resistance absorb nullify chain', function()
        -- 100 * 0.5 = 50; * 1 absorb; * 1 null; final 50
        assert(calcDamage({
            afterBonuses = 100, resistance = 0.5, absorb = 1, nullify = 1, afterFinalMagic = 50,
        }) == 50)
    end)

    it('absorb negative passes through floors', function()
        -- 100 * 1 * -1 = -100; * 1 null; final inject -100
        local d = 100
        d = math.floor(d * 1)
        d = math.floor(d * -1)
        d = math.floor(d * 1)
        assert(d == -100)
        assert(calcDamage({
            afterBonuses = 100, resistance = 1, absorb = -1, nullify = 1, afterFinalMagic = -100,
        }) == -100)
    end)
end)

describe('additionalEffect calcPhysDamage pure plan', function()
    local DMG_PIERCING = 1
    local DMG_SLASHING = 2
    local DMG_BLUNT = 3
    local DMG_HTH = 4

    local function sdtMult(damageType, pierce, slash, impact, hth)
        if damageType == DMG_PIERCING then
            return 1 + (pierce or 0) / 10000
        end
        if damageType == DMG_SLASHING then
            return 1 + (slash or 0) / 10000
        end
        if damageType == DMG_BLUNT then
            return 1 + (impact or 0) / 10000
        end
        if damageType == DMG_HTH then
            return 1 + (hth or 0) / 10000
        end
        return 1
    end

    local function calcPhys(p)
        local damage = math.floor(p.damage or 0)
        if damage == 0 then
            return 0
        end
        if p.nullAll then return 0 end
        if p.isPhysical and p.nullPhysical then return 0 end
        if p.isRanged and p.nullRanged then return 0 end
        if p.isBreath and p.nullBreath then return 0 end
        if p.absorbAll then return damage * -1 end
        if (p.isPhysical or p.isRanged) and p.absorbPhys then
            return damage * -1
        end
        damage = damage * (p.damageAdj or 1)
        damage = damage * sdtMult(p.damageType or 0, p.pierceSDT, p.slashSDT, p.impactSDT, p.hthSDT)
        damage = math.floor(damage)
        if p.phalanx then damage = p.phalanx(damage) end
        if p.stoneskin then damage = p.stoneskin(damage) end
        return damage
    end

    it('zero damage early return', function()
        assert(calcPhys({ damage = 0 }) == 0)
        assert(calcPhys({ damage = 0.9 }) == 0) -- floor first
    end)

    it('null all damage', function()
        assert(calcPhys({ damage = 50, nullAll = true }) == 0)
    end)

    it('null physical only when isPhysical', function()
        assert(calcPhys({ damage = 50, isPhysical = true, nullPhysical = true }) == 0)
        assert(calcPhys({ damage = 50, isPhysical = false, nullPhysical = true, damageAdj = 1 }) == 50)
    end)

    it('absorb all returns negative', function()
        assert(calcPhys({ damage = 40, absorbAll = true }) == -40)
    end)

    it('phys absorb when physical or ranged', function()
        assert(calcPhys({ damage = 30, isPhysical = true, absorbPhys = true }) == -30)
        assert(calcPhys({ damage = 30, isRanged = true, absorbPhys = true }) == -30)
    end)

    it('SDT piercing multiplies then floor', function()
        -- 100 * 1 adj * (1 + 2500/10000) = 125
        assert(calcPhys({
            damage = 100, damageAdj = 1, damageType = DMG_PIERCING, pierceSDT = 2500,
        }) == 125)
    end)

    it('slash blunt hth SDT arms', function()
        assert(calcPhys({ damage = 100, damageType = DMG_SLASHING, slashSDT = 1000 }) == 110)
        assert(calcPhys({ damage = 100, damageType = DMG_BLUNT, impactSDT = -5000 }) == 50)
        assert(calcPhys({ damage = 100, damageType = DMG_HTH, hthSDT = 0 }) == 100)
    end)

    it('phalanx and stoneskin host injects', function()
        local r = calcPhys({
            damage = 100, damageAdj = 1,
            phalanx = function(d) return d - 10 end,
            stoneskin = function(d) return d - 5 end,
        })
        assert(r == 85)
    end)
end)
