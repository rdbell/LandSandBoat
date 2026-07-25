-----------------------------------
-- Pure system tests for calculateAttackDamage dual-wire (slice 6760).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/attack.CalculateAttackDamage (1377/1577).
-- Note: DA/TA dmg mods use additive production quirk (not Lua multiplicative).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

local function almost(a, b)
    return math.abs(a - b) < 1e-6
end

describe('attack damage component pure pins', function()
    it('SA/TA bonuses and natural H2H', function()
        assert(almost(p.sneakAttackDexBonusFromParams({
            dex = 100, sneakAtkDexMod = 0,
        }), 100))
        assert(almost(p.sneakAttackDexBonusFromParams({
            dex = 100, sneakAtkDexMod = 50,
        }), 150))
        assert(almost(p.trickAttackAgiBonusFromParams({
            agi = 80, trickAtkAgiMod = 25,
        }), 100))
        -- floor(100*0.11)+3 = 14
        assert(p.naturalH2hDamageFromParams({ h2hSkill = 100 }) == 14)
        assert(almost(p.mobH2HPenaltyFromParams({
            noH2HPenaltyMod = true,
        }), 1))
        assert(almost(p.mobH2HPenaltyFromParams({
            isPreToAURegion = true,
        }), 0.425))
        assert(almost(p.mobH2HPenaltyFromParams({
            isPreToAURegion = false,
        }), 0.65))
    end)

    it('additive DA/TA production quirk', function()
        -- floor(100 * 1 + 0.20) = 100
        assert(p.applyDoubleTripleAttackDamageFromParams({
            damage = 100, dmgMod = 20,
        }) == 100)
        assert(p.applyDoubleTripleAttackDamageFromParams({
            damage = 100, dmgMod = 150,
        }) == 101)
    end)
end)

describe('calculateAttackDamageFromParams product', function()
    it('main hand baseline', function()
        -- weapon 50 + fSTR 5 = 55; ratio 1.5 → floor(82.5)=82
        assert(p.calculateAttackDamageFromParams({
            slot = xi.slot.MAIN, weaponDmg = 50, fSTR = 5,
            damageRatio = 1.5, scarletMult = 1.0,
        }) == 82)
    end)

    it('SA bonus on main hand', function()
        -- DEX 100 → +100; 50+100 = 150 * 1 = 150
        assert(p.calculateAttackDamageFromParams({
            isSneakAttack = true, dex = 100,
            slot = xi.slot.MAIN, weaponDmg = 50,
            damageRatio = 1.0, scarletMult = 1.0,
        }) == 150)
    end)

    it('player H2H punch and kick', function()
        assert(p.calculateAttackDamageFromParams({
            isH2H = true, naturalH2h = 14, weaponDmg = 10, fSTR = 2,
            damageRatio = 1.0, scarletMult = 1.0,
        }) == 26)
        assert(p.calculateAttackDamageFromParams({
            isH2H = true, isKick = true, naturalH2h = 14,
            kickDamageMod = 5, fSTR = 2,
            damageRatio = 1.0, scarletMult = 1.0,
        }) == 21)
    end)

    it('mob H2H with penalty', function()
        -- (40+0+4)*0.65 = 28.6 → floor 28 as int then *1
        assert(p.calculateAttackDamageFromParams({
            isH2H = true, isMob = true, weaponDmg = 40, fSTR = 4,
            mobH2HPenalty = 0.65, damageRatio = 1.0, scarletMult = 1.0,
        }) == 28)
    end)

    it('ammo ignores SA bonus base', function()
        assert(p.calculateAttackDamageFromParams({
            isSneakAttack = true, dex = 100,
            slot = xi.slot.AMMO, weaponDmg = 50, fSTR = 5,
            damageRatio = 1.0, scarletMult = 1.0,
        }) == 55)
    end)

    it('scarlet and SA augment', function()
        -- 100 * 1.1 = 110; SA augment 20% → floor(110*1.2)=132
        assert(p.calculateAttackDamageFromParams({
            isSneakAttack = true, dex = 0,
            slot = xi.slot.MAIN, weaponDmg = 100,
            damageRatio = 1.0, scarletMult = 1.1,
            augmentsSA = 20, hasSneakAttackEffect = true,
        }) == 132)
    end)

    it('clamps negative fSTR crater to zero', function()
        assert(p.calculateAttackDamageFromParams({
            slot = xi.slot.MAIN, weaponDmg = 5, fSTR = -20,
            damageRatio = 1.0, scarletMult = 1.0,
        }) == 0)
    end)

    it('PC double-attack additive dmg mod', function()
        assert(p.calculateAttackDamageFromParams({
            slot = xi.slot.MAIN, weaponDmg = 100,
            damageRatio = 1.0, scarletMult = 1.0,
            attackType = xi.physicalAttackType.DOUBLE,
            isPC = true, doubleAttackDmg = 20,
        }) == 100)
        assert(p.calculateAttackDamageFromParams({
            slot = xi.slot.MAIN, weaponDmg = 100,
            damageRatio = 1.0, scarletMult = 1.0,
            attackType = xi.physicalAttackType.DOUBLE,
            isPC = true, doubleAttackDmg = 150,
        }) == 101)
    end)
end)
