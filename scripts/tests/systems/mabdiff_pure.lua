-----------------------------------
-- Pure system tests for MAB/MDB ratio dual-wire helpers (slice 6707).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/mabdiff (0859).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local spell = xi.magic.spell
local skill = xi.skill
local el = xi.element
local job = xi.job

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('MAB pure constants and band gates', function()
    it('pins crit / theurgic / ratio bounds', function()
        assert(dmg.mabCritAddMin == 10)
        assert(dmg.mabCritAddMax == 40)
        assert(dmg.theurgicFocusBase == 50)
        assert(dmg.theurgicFocusJPScale == 3)
        assert(dmg.mabRatioMin == 0)
        assert(dmg.mabRatioMax == 10)
    end)

    it('isAncientMagic FLARE..FLOOD_II', function()
        assert(dmg.isAncientMagic(spell.FLARE))
        assert(dmg.isAncientMagic(spell.FLOOD_II))
        assert(not dmg.isAncientMagic(spell.FLARE - 1))
        assert(not dmg.isAncientMagic(spell.FLOOD_II + 1))
    end)

    it('isTheurgicRa -ra bands', function()
        assert(dmg.isTheurgicRa(spell.FIRA))
        assert(dmg.isTheurgicRa(spell.WATERA_II))
        assert(dmg.isTheurgicRa(spell.FIRA_III))
        assert(dmg.isTheurgicRa(spell.WATERA_III))
        assert(not dmg.isTheurgicRa(spell.FIRA - 1))
    end)

    it('mabCritAdd clamps 10..40', function()
        assert(dmg.mabCritAdd(0) == 10)
        assert(dmg.mabCritAdd(20) == 30)
        assert(dmg.mabCritAdd(100) == 40)
        assert(dmg.mabCritAdd(-100) == 10)
    end)
end)

describe('elementalNinMeritFromParams', function()
    it('selects band merit', function()
        assert(dmg.elementalNinMeritFromParams({
            spellId = spell.KATON_ICHI, katonEffectMerit = 10, hyotonEffectMerit = 99,
        }) == 10)
        assert(dmg.elementalNinMeritFromParams({
            spellId = spell.SUITON_SAN, suitonEffectMerit = 8,
        }) == 8)
        assert(dmg.elementalNinMeritFromParams({
            spellId = spell.FLARE, katonEffectMerit = 10,
        }) == 0)
    end)
end)

describe('calculateMagicBonusDiffFromParams', function()
    it('neutral base is 1.0', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({}), 1.0))
    end)

    it('base MAB and target MDEF', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            baseMAB = 50, targetMDEF = 25,
        }), 1.2))
    end)

    it('magic crit add', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            mabCritProc = true, magicCritDmgIncrease = 0,
        }), 1.1))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            mabCritProc = true, magicCritDmgIncrease = 20,
        }), 1.3))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            mabCritProc = false, magicCritDmgIncrease = 40,
        }), 1.0))
    end)

    it('ninjutsu merits only for NINJUTSU skill', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            skillType = skill.ELEMENTAL_MAGIC, spellId = spell.KATON_ICHI,
            ninMagicBonusMerit = 20, katonEffectMerit = 10, ninNukeBonusGear = 5,
        }), 1.0))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            skillType = skill.NINJUTSU, spellId = spell.KATON_ICHI,
            ninMagicBonusMerit = 20, katonEffectMerit = 10, ninNukeBonusGear = 5,
        }), 1.35))
    end)

    it('elemental potency and barspell MDB', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            spellElement = el.FIRE, elementalPotencyMerit = 10, barspellSubPower = 20,
        }), 110 / 120))
        -- light: no potency/bar
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            spellElement = el.LIGHT, elementalPotencyMerit = 10, barspellSubPower = 20,
        }), 1.0))
    end)

    it('RDM/GEO job points', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            mainJob = job.RDM, rdmJobPointMAB = 15,
        }), 1.15))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            mainJob = job.GEO, geoJobPointMAB = 10,
        }), 1.10))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            mainJob = job.BLM, rdmJobPointMAB = 15, geoJobPointMAB = 10,
        }), 1.0))
    end)

    it('ancient magic and theurgic focus', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            spellId = spell.FLARE, ancientMagicAtkBonusMerit = 20,
        }), 1.20))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            spellId = spell.FIRA, hasTheurgicFocus = true, theurgicFocusJP = 2,
        }), 1.56)) -- +50 + 6 = 56 → 156/100
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            spellId = spell.FIRA, hasTheurgicFocus = false, theurgicFocusJP = 5,
        }), 1.0))
    end)

    it('auto MAB coefficient and clamp', function()
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            baseMAB = 0, autoMABCoefficient = 50,
        }), 1.5))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            baseMAB = 1000, targetMDEF = 0,
        }), 10.0))
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            baseMAB = -100, targetMDEF = 0,
        }), 0.0)) -- finalCasterMAB=0 → 0/100
        assert(almost(dmg.calculateMagicBonusDiffFromParams({
            baseMAB = -200, targetMDEF = -100, -- MDB=0, MAB negative
        }), 0.0))
    end)
end)
