-----------------------------------
-- Pure system tests for enhancing-song dual-wire (slice 6729).
-- Calls production xi.spells.enhancing song pure exports.
-- Goldens match internal/enhancingsong (0875 / 6115).
-----------------------------------

require('scripts/globals/spells/enhancing_song')

local e = xi.spells.enhancing

describe('Enhancing song pure pins', function()
    it('base duration and skill constants', function()
        assert(e.baseSongDurationSeconds == 120)
        assert(e.skillSinging == 40)
        assert(e.skillStringInstrument == 41)
        assert(e.skillWindInstrument == 42)
    end)
end)

describe('resolveSingingLevelFromParams', function()
    it('PC wind string and non-PC', function()
        assert(e.resolveSingingLevelFromParams({
            isPC = false, singingSkill = 100,
        }) == 200)
        assert(e.resolveSingingLevelFromParams({
            isPC = true, singingSkill = 100,
            rangedSkillType = xi.skill.WIND_INSTRUMENT, rangedSkillLevel = 50,
        }) == 150)
        assert(e.resolveSingingLevelFromParams({
            isPC = true, singingSkill = 100,
            rangedSkillType = xi.skill.STRING_INSTRUMENT, rangedSkillLevel = 51,
        }) == 125)
        assert(e.resolveSingingLevelFromParams({
            isPC = true, singingSkill = 100, rangedSkillType = 0, rangedSkillLevel = 99,
        }) == 100)
    end)
end)

describe('calculateSongPowerFromParams', function()
    it('Etude tier ladders', function()
        assert(e.calculateSongPowerFromParams({
            powerBase = 3, tier = 1, songEffect = xi.effect.ETUDE,
            potencyCap = 9, multiplier = 1, singingLvl = 450,
        }) == 9)
        assert(e.calculateSongPowerFromParams({
            powerBase = 3, tier = 1, songEffect = xi.effect.ETUDE,
            potencyCap = 9, multiplier = 1, singingLvl = 182,
        }) == 4)
        assert(e.calculateSongPowerFromParams({
            powerBase = 3, tier = 1, songEffect = xi.effect.ETUDE,
            potencyCap = 9, multiplier = 1, singingLvl = 181,
        }) == 3)
        assert(e.calculateSongPowerFromParams({
            powerBase = 12, tier = 2, songEffect = xi.effect.ETUDE,
            potencyCap = 15, multiplier = 1, singingLvl = 475,
        }) == 15)
        -- SV + augment: floor(3*2)+2 = 8
        assert(e.calculateSongPowerFromParams({
            powerBase = 3, tier = 1, songEffect = xi.effect.ETUDE,
            potencyCap = 9, multiplier = 1, singingLvl = 0,
            soulVoiceAffectsPower = true, hasSoulVoice = true, augmentSongStat = 2,
        }) == 8)
    end)

    it('skill divisor Paeon Ballad instrument merit JP SV', function()
        -- Minuet-like: floor(5+(163-50)/4.3)=31
        assert(e.calculateSongPowerFromParams({
            powerBase = 5, skillNeeded = 50, potencyCap = 32,
            multiplier = 3, divisor = 4.3, singingLvl = 163,
        }) == 31)
        -- over cap
        assert(e.calculateSongPowerFromParams({
            powerBase = 5, skillNeeded = 50, potencyCap = 32,
            multiplier = 3, divisor = 4.3, singingLvl = 500,
        }) == 32)
        -- Paeon +1
        assert(e.calculateSongPowerFromParams({
            powerBase = 1, skillNeeded = 100, potencyCap = 2,
            multiplier = 1, divisor = 0, singingLvl = 150,
        }) == 2)
        -- Ballad constant
        assert(e.calculateSongPowerFromParams({
            powerBase = 2, skillNeeded = 0, potencyCap = 2,
            multiplier = 1, divisor = 0, singingLvl = 999,
        }) == 2)
        -- instrument 2 * mult 6 = 12 → floor(10+12)=22 +5 +3 = 30
        assert(e.calculateSongPowerFromParams({
            powerBase = 10, potencyCap = 100, multiplier = 6, singingLvl = 0,
            instrumentBoost = 2, meritBonus = 5, jobPointBonus = 3,
            soulVoiceAffectsPower = true,
        }) == 30)
        -- SV ×2 → 60
        assert(e.calculateSongPowerFromParams({
            powerBase = 10, potencyCap = 100, multiplier = 6,
            instrumentBoost = 2, meritBonus = 5, jobPointBonus = 3,
            soulVoiceAffectsPower = true, hasSoulVoice = true,
        }) == 60)
        -- Marcato 50%: floor(30 * 1.5)=45
        assert(e.calculateSongPowerFromParams({
            powerBase = 10, potencyCap = 100, multiplier = 6,
            instrumentBoost = 2, meritBonus = 5, jobPointBonus = 3,
            soulVoiceAffectsPower = true, hasMarcato = true, marcatoPower = 50,
        }) == 45)
    end)
end)

describe('calculateSongDurationFromParams', function()
    it('gear JP status products', function()
        local r = e.calculateSongDurationFromParams({})
        assert(r.duration == 120 and not r.consumeMarcato)

        -- instrument 2 → floor(120 * 1.2) = 144
        r = e.calculateSongDurationFromParams({ instrumentBoost = 2 })
        assert(r.duration == 144)

        -- SONG_DURATION_BONUS 10% → floor(120 * 1.1) = 132
        r = e.calculateSongDurationFromParams({ songDurationBonus = 10 })
        assert(r.duration == 132)

        -- Clarion Call JP 10 → +20
        r = e.calculateSongDurationFromParams({ hasClarionCall = true, clarionCallJP = 10 })
        assert(r.duration == 140)

        -- Marcato JP 5 consume
        r = e.calculateSongDurationFromParams({ hasMarcato = true, marcatoJP = 5 })
        assert(r.duration == 125 and r.consumeMarcato)

        -- Tenuto JP 5 → +10
        r = e.calculateSongDurationFromParams({ hasTenuto = true, tenutoJP = 5 })
        assert(r.duration == 130)

        -- SV duration when power not affected: ×2
        r = e.calculateSongDurationFromParams({
            soulVoiceAffectsPower = false, hasSoulVoice = true,
        })
        assert(r.duration == 240)

        -- SV does not scale duration when power is affected
        r = e.calculateSongDurationFromParams({
            soulVoiceAffectsPower = true, hasSoulVoice = true,
        })
        assert(r.duration == 120)

        -- Troubadour ×2
        r = e.calculateSongDurationFromParams({ hasTroubadour = true })
        assert(r.duration == 240)
    end)
end)

describe('use helpers and plan', function()
    it('subEffect paramFour marchPower', function()
        assert(e.resolveSubEffect(xi.effect.CAROL, 1, 2, 0) == 1 + 200)
        assert(e.resolveSubEffect(xi.effect.ETUDE, 5, 0, 0) == 5)
        assert(e.resolveSubEffect(xi.effect.MINUET, 10, 0, 7) == 7)
        assert(e.paramFourFor(xi.effect.ETUDE, 2) == 10)
        assert(e.paramFourFor(xi.effect.ETUDE, 1) == 0)
        assert(e.paramFourFor(xi.effect.MARCH, 1) == 0)
        assert(e.marchPower(1024) == 10000)
        assert(e.marchPower(512) == 5000)
    end)

    it('useEnhancingSongFromParams plan', function()
        local plan = e.useEnhancingSongFromParams({
            songEffect = xi.effect.MINUET, tier = 1, power = 20, duration = 120,
            tableSub = 10, subModValue = 3, addBardSongOK = true,
        })
        assert(plan.applySong and plan.power == 20 and plan.duration == 120)
        assert(plan.subEffect == 3 and plan.paramFour == 0)
        assert(not plan.setMsg)

        plan = e.useEnhancingSongFromParams({
            songEffect = xi.effect.MARCH, tier = 1, power = 1024, duration = 120,
            addBardSongOK = true,
        })
        assert(plan.power == 10000 and plan.applySong)

        plan = e.useEnhancingSongFromParams({
            songEffect = xi.effect.ETUDE, tier = 2, power = 15, duration = 120,
            tableSub = xi.mod.STR, addBardSongOK = false, hasMarcato = true,
        })
        assert(not plan.applySong and plan.setMsg and plan.msg == 75)
        assert(plan.paramFour == 10 and plan.consumeMarcato)
        assert(plan.subEffect == xi.mod.STR)
    end)
end)
