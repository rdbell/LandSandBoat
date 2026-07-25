-----------------------------------
-- Pure system tests for enfeebling dual-wire (slice 6727).
-- Calls production xi.spells.enfeebling pure exports.
-- Goldens match internal/enfeebling (0872 / 6089).
-----------------------------------

require('scripts/globals/spells/enfeebling_spell')

local e = xi.spells.enfeebling

describe('Enfeebling pure pins', function()
    it('skill and saboteur / immunobreak constants', function()
        assert(e.skillEnfeeblingMagic == 35)
        assert(e.saboteurNMMult == 1.3)
        assert(e.saboteurNormalMult == 2.0)
        assert(e.immunobreakMinBaseRank == 6)
        assert(e.immunobreakMinFinalRank == 4)
        assert(e.immunobreakBaseChanceFlat == 20)
    end)
end)

describe('elementalDebuffPotencyFromParams', function()
    it('stat ladder and merit/mod', function()
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 0 }) == 1)
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 40 }) == 1)
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 41 }) == 2)
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 71 }) == 3)
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 101 }) == 4)
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 151 }) == 5)
        -- merit 2 + mod 5 → +2.5; base 3 at stat 80 → 7.5
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 80, merit = 2, mod = 5 }) == 7.5)
        assert(e.elementalDebuffPotencyFromParams({ casterStat = 0, mod = 11 }) == 6.5)
    end)
end)

describe('calculatePotencyFromParams', function()
    it('addle blind paralysis slow ladders', function()
        -- Addle: base 20 + clamp(floor(diff/5),0,20); diff 30 → 26
        assert(e.calculatePotencyFromParams({
            spellEffect = xi.effect.ADDLE, basePotency = 20,
            casterStat = 50, targetStat = 20,
        }) == 26)

        -- Blind I: clamp(diff*0.225+23,5,50); diff 20 → 27
        assert(e.calculatePotencyFromParams({
            spellId = xi.magic.spell.BLIND, spellEffect = xi.effect.BLINDNESS,
            casterStat = 50, targetMND = 30,
        }) == 27)

        -- Blind II: clamp(diff*0.375+49,19,94); diff 20 → 56
        assert(e.calculatePotencyFromParams({
            spellId = xi.magic.spell.BLIND_II, spellEffect = xi.effect.BLINDNESS,
            casterStat = 50, targetMND = 30,
        }) == 56)

        -- Paralyze I: clamp(diff/4+15,5,25); diff 20 → 20
        assert(e.calculatePotencyFromParams({
            spellId = xi.magic.spell.PARALYZE, spellEffect = xi.effect.PARALYSIS,
            casterStat = 50, targetStat = 30,
        }) == 20)

        -- Slow I: clamp(diff*73/5+1825,730,2920); diff 0 → 1825
        assert(e.calculatePotencyFromParams({
            spellId = xi.magic.spell.SLOW, spellEffect = xi.effect.SLOW,
            casterStat = 0, targetStat = 0,
        }) == 1825)
    end)

    it('poison skill bands and fixed passthrough', function()
        -- Poison I skill 100 → max(4,1)=4
        assert(e.calculatePotencyFromParams({
            spellId = xi.magic.spell.POISON, spellEffect = xi.effect.POISON,
            skillLevel = 100,
        }) == 4)

        -- Poison I skill 500 → min((500-225)/5,55)=55
        assert(e.calculatePotencyFromParams({
            spellId = xi.magic.spell.POISON, spellEffect = xi.effect.POISON,
            skillLevel = 500,
        }) == 55)

        -- Sleep fixed base
        assert(e.calculatePotencyFromParams({
            spellEffect = xi.effect.SLEEP_I, basePotency = 1,
        }) == 1)
    end)

    it('saboteur and enf mag potency', function()
        -- base 10, saboteur non-NM ×2, no enhance → 20
        assert(e.calculatePotencyFromParams({
            spellEffect = xi.effect.SLEEP_I, basePotency = 10,
            applySaboteur = true, hasSaboteur = true, targetIsNM = false,
            skillType = e.skillEnfeeblingMagic,
        }) == 20)

        -- NM saboteur ×1.3 → floor(10*1.3)=13
        assert(e.calculatePotencyFromParams({
            spellEffect = xi.effect.SLEEP_I, basePotency = 10,
            applySaboteur = true, hasSaboteur = true, targetIsNM = true,
            skillType = e.skillEnfeeblingMagic,
        }) == 13)

        -- ENF_MAG_POTENCY 50%: floor(10*1.5)=15
        assert(e.calculatePotencyFromParams({
            spellEffect = xi.effect.SLEEP_I, basePotency = 10, enfMagPotency = 50,
        }) == 15)
    end)

    it('elemental via injects', function()
        assert(e.calculatePotencyFromParams({
            spellEffect = xi.effect.BURN, casterStat = 151,
            elementalDebuffMerit = 0, elementalDebuffMod = 0,
        }) == 5)
    end)
end)

describe('calculateDurationFromParams', function()
    it('bind inject elemental helix saboteur rdm', function()
        -- Bind with inject 40
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.BIND, baseDuration = 60, bindDuration = 40,
        }) == 40)

        -- Elemental + merit
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.BURN, baseDuration = 90, elementalDebuffDurationMerit = 10,
        }) == 100)

        -- Helix level 60 + dark arts JP 2 + mod 5 → 90+60+6+5=161
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.HELIX, baseDuration = 90,
            casterMainLvl = 60, hasDarkArts = true, darkArtsJP = 2, helixDurationMod = 5,
        }) == 161)

        -- Helix level 40 → +30
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.HELIX, baseDuration = 90, casterMainLvl = 40,
        }) == 120)

        -- Enfeebling saboteur non-NM ×2 on base 120 → 240
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.SLOW, baseDuration = 120,
            skillType = e.skillEnfeeblingMagic, hasSaboteur = true, targetIsNM = false,
        }) == 240)

        -- NM saboteur ×1.25 → 150
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.SLOW, baseDuration = 120,
            skillType = e.skillEnfeeblingMagic, hasSaboteur = true, targetIsNM = true,
        }) == 150)

        -- RDM merit/JP/stymie + enf mag duration %
        -- base 100, rdm +10 merit +5 jp + stymie 3 = 118; *1.1 enf = floor(129.8)=129
        assert(e.calculateDurationFromParams({
            spellEffect = xi.effect.SLOW, baseDuration = 100,
            skillType = e.skillEnfeeblingMagic,
            isRDM = true, enfeeblingDurationMerit = 10, enfeebleDurationJP = 5,
            hasStymie = true, stymieJP = 3, enfMagDuration = 10,
        }) == 129)
    end)
end)

describe('immunobreak pure', function()
    it('eligible gates', function()
        assert(e.immunobreakEligibleFromParams({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 0,
        }))
        assert(not e.immunobreakEligibleFromParams({
            enabled = false, casterIsPC = true, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 0,
        }))
        assert(not e.immunobreakEligibleFromParams({
            enabled = true, casterIsPC = false, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 0,
        }))
        assert(not e.immunobreakEligibleFromParams({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 0,
            baseResistanceRank = 6, immunobreakValue = 0,
        }))
        assert(not e.immunobreakEligibleFromParams({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 100,
            baseResistanceRank = 5, immunobreakValue = 0,
        }))
        -- final rank <= 4 blocked: base 6 value 2 → 4
        assert(not e.immunobreakEligibleFromParams({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 2,
        }))
        -- base 7 value 2 → 5 > 4 ok
        assert(e.immunobreakEligibleFromParams({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = e.skillEnfeeblingMagic, immunobreakModId = 100,
            baseResistanceRank = 7, immunobreakValue = 2,
        }))
    end)

    it('chance and roll', function()
        assert(e.immunobreakChance(0, 0) == 20)
        assert(e.immunobreakChance(10, 1) == 20) -- 10 + 20/2 = 20
        assert(e.immunobreakChance(0, 3) == 5)
        assert(e.immunobreakSucceeds(20, 20))
        assert(e.immunobreakSucceeds(20, 1))
        assert(not e.immunobreakSucceeds(20, 21))
    end)
end)
