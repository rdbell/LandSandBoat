-----------------------------------
-- Pure system tests for base spell damage dual-wire (slice 6713).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/spellbasedmg (0857) aeroI-style tables.
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local spell = xi.magic.spell
local skill = xi.skill
local spellGroup = xi.magic.spellGroup

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

-- Aero I-like row: V=25, M=1, vPC=40, I=35, M0=1.6, M50=1
local function aeroIParams(extra)
    local p =
    {
        npcPower       = 25,
        npcMultiplier  = 1,
        pcPower        = 40,
        inflexionPoint = 35,
        multiplier0    = 1.6,
        multiplier50   = 1,
        multiplier100  = 0,
        multiplier200  = 0,
        multiplier300  = 0,
        multiplier400  = 0,
        multiplier500  = 0,
    }
    if extra then
        for k, v in pairs(extra) do
            p[k] = v
        end
    end

    return p
end

describe('Base damage constants and system gate', function()
    it('pins clamp bounds', function()
        assert(dmg.baseDamageMin == 0)
        assert(dmg.baseDamageMax == 99999)
    end)

    it('useNewMagicDamageSystem gate', function()
        assert(not dmg.useNewMagicDamageSystem({ multiplier0 = 1.6, isPC = false }))
        assert(not dmg.useNewMagicDamageSystem({ multiplier0 = 0, isPC = true }))
        assert(not dmg.useNewMagicDamageSystem({
            multiplier0 = 1.6, isPC = true, useOldMagicDamage = true,
        }))
        assert(dmg.useNewMagicDamageSystem({
            multiplier0 = 1.6, isPC = true, useOldMagicDamage = false,
        }))
    end)
end)

describe('oldSystemStatDiffBonus', function()
    it('negative, below, above inflexion, and cap', function()
        assert(dmg.oldSystemStatDiffBonus(-10, 1, 35) == -10)
        assert(dmg.oldSystemStatDiffBonus(20, 1, 35) == 20)
        -- floor(35)+floor(15/2)=35+7
        assert(dmg.oldSystemStatDiffBonus(50, 1, 35) == 42)
        -- cap 3*I=105: same as 105
        assert(dmg.oldSystemStatDiffBonus(200, 1, 35) == dmg.oldSystemStatDiffBonus(105, 1, 35))
    end)
end)

describe('newSystemStatDiffBonus', function()
    it('segments M0 and M50', function()
        -- aeroI: M0=1.6 width 50, M50=1 width 50
        local mult = { 1.6, 1, 0, 0, 0, 0, 0 }
        -- statDiff 30: floor(30*1.6)=48
        assert(dmg.newSystemStatDiffBonus(30, mult) == 48)
        -- statDiff 80: floor(50*1.6)+floor(30*1)=80+30=110
        assert(dmg.newSystemStatDiffBonus(80, mult) == 110)
    end)
end)

describe('baseSpellDamageBonusFromParams', function()
    it('PC JP/status and gear/cascade', function()
        assert(dmg.baseSpellDamageBonusFromParams({
            isPC = true, hasManafont = true, manafontJP = 5,
        }) == 15)
        assert(dmg.baseSpellDamageBonusFromParams({
            isPC = true, hasManawell = true, manawellJP = 7,
        }) == 7)
        assert(dmg.baseSpellDamageBonusFromParams({
            isPC = true, isBLMMain = true, magicDmgBonusJP = 10,
        }) == 10)
        assert(dmg.baseSpellDamageBonusFromParams({
            isPC = true, skillType = skill.NINJUTSU, elemNinjutsuJP = 4,
        }) == 8)
        assert(dmg.baseSpellDamageBonusFromParams({
            isPC = true, spellGroup = spellGroup.BLACK, hasEbullience = true,
            strategemEffectIIIJP = 3,
        }) == 6)
        assert(dmg.baseSpellDamageBonusFromParams({
            magicDamageMod = 40,
        }) == 40)
        assert(dmg.baseSpellDamageBonusFromParams({
            skillType = skill.ELEMENTAL_MAGIC, hasCascade = true, currentTP = 1250,
        }) == 125)
    end)
end)

describe('calculateBaseDamageFromParams', function()
    it('old system zero and stat ladder', function()
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({ isPC = false })), 25))
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            isPC = false, statDiff = -10,
        })), 15))
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            isPC = false, statDiff = 20,
        })), 45))
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            isPC = false, statDiff = 50,
        })), 67))
    end)

    it('new system PC V and ladder', function()
        -- V=40, stat 30 → 40+48=88
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            isPC = true, statDiff = 30,
        })), 88))
    end)

    it('Death exception V + MP*3', function()
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            spellId = spell.DEATH, isPC = true, casterMP = 100, magicDamageMod = 50, statDiff = 999,
        })), 340))
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            spellId = spell.DEATH, isPC = false, casterMP = 10,
        })), 55))
    end)

    it('Helix exception adds HELIX_EFFECT', function()
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            spellId = spell.GEOHELIX, isPC = true, helixEffectMod = 15,
        })), 55))
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            spellId = spell.LUMINOHELIX_II, isPC = true, helixEffectMod = 8,
        })), 48))
    end)

    it('Kaustra rebuild from level', function()
        -- L75: floor(50.25)=50 → V=5; old bonus floor with I=35 M=1 stat 40 → 37; mdmg 10
        -- floor(5*(10+37))=235
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            spellId = spell.KAUSTRA, isPC = false, statDiff = 40,
            magicDamageMod = 10, mainLvl = 75,
        })), 235))
        assert(almost(dmg.calculateBaseDamageFromParams(aeroIParams({
            spellId = spell.KAUSTRA, mainLvl = 1,
        })), 0))
    end)

    it('clamps to 0..99999', function()
        assert(dmg.calculateBaseDamageFromParams(aeroIParams({
            isPC = false, statDiff = -100000,
        })) == 0)
        assert(dmg.calculateBaseDamageFromParams(aeroIParams({
            isPC = true, magicDamageMod = 200000,
        })) == 99999)
    end)
end)
