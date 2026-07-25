-----------------------------------
-- Pure system tests for enfeebling-song dual-wire (slice 6728).
-- Calls production xi.spells.enfeebling song pure exports.
-- Goldens match internal/enfeeblingsong (0873 / 6114 helpers).
-----------------------------------

require('scripts/globals/spells/enfeebling_song')

local e = xi.spells.enfeebling

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('calculateSongPowerFromParams', function()
    it('Requiem gear clamp and JP', function()
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.REQUIEM, basePower = 1, gearBoost = 0,
        }) == 1)
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.REQUIEM, basePower = 1, gearBoost = 1,
        }) == 1)
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.REQUIEM, basePower = 1, gearBoost = 5,
        }) == 5)
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.REQUIEM, basePower = 8, gearBoost = 30,
        }) == 28)
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.REQUIEM, basePower = 1, requiemJP = 5,
        }) == 16)
    end)

    it('Elegy Threnody Nocturne ladders', function()
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.ELEGY, basePower = 2500, gearBoost = 0,
        }) == 2500)
        assert(almost(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.ELEGY, basePower = 2500, gearBoost = 1,
        }), 2500 + 6375 / 256))
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.THRENODY, basePower = 50, gearBoost = 3,
        }) == 65)
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.NOCTURNE, basePower = 15, gearBoost = 4,
        }) == 21)
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.NOCTURNE, basePower = 15, gearBoost = 1,
        }) == 16.5)
    end)

    it('passthrough and Soul Voice / Marcato', function()
        -- Lullaby ignores SV
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.SLEEP_I, basePower = 1, gearBoost = 10, hasSoulVoice = true,
        }) == 1)
        -- Requiem SV ×2
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.REQUIEM, basePower = 5, hasSoulVoice = true,
        }) == 10)
        -- SV over Marcato
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.THRENODY, basePower = 50,
            hasSoulVoice = true, hasMarcato = true, marcatoPower = 50,
        }) == 100)
        -- Marcato only ×1.5
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.THRENODY, basePower = 50,
            hasMarcato = true, marcatoPower = 50,
        }) == 75)
        -- Nocturne fractional then SV
        assert(e.calculateSongPowerFromParams({
            spellEffect = xi.effect.NOCTURNE, basePower = 15, gearBoost = 1, hasSoulVoice = true,
        }) == 33)
    end)
end)

describe('calculateSongDurationFromParams', function()
    it('Virelai early path', function()
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.CHARM_I, baseDuration = 30,
        }) == 30)
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.CHARM_I, baseDuration = 30, gearBoost = 3,
        }) == 39)
        -- SONG_DURATION_BONUS and Troubadour ignored
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.CHARM_I, baseDuration = 30,
            songDurationBonus = 50, hasTroubadour = true,
            hasClarionCall = true, clarionCallJP = 10,
        }) == 30)
    end)

    it('gear bonus lullaby JP status products', function()
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.ELEGY, baseDuration = 120,
        }) == 120)
        -- gear 2 → floor(120 * 1.2) = 144
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.ELEGY, baseDuration = 120, gearBoost = 2,
        }) == 144)
        -- SONG_DURATION_BONUS 10%: floor(120 * 1.1) = 132
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.ELEGY, baseDuration = 120, songDurationBonus = 10,
        }) == 132)
        -- Lullaby JP +5 after gear product: floor(30*1.0)+5 = 35
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.SLEEP_I, baseDuration = 30, lullabyJP = 5,
        }) == 35)
        -- Clarion Call JP 10 → +20
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.ELEGY, baseDuration = 100,
            hasClarionCall = true, clarionCallJP = 10,
        }) == 120)
        -- Tenuto JP 5 → +10
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.ELEGY, baseDuration = 100,
            hasTenuto = true, tenutoJP = 5,
        }) == 110)
        -- Troubadour ×2 after flats
        assert(e.calculateSongDurationFromParams({
            spellEffect = xi.effect.ELEGY, baseDuration = 100,
            hasTroubadour = true,
        }) == 200)
    end)
end)

describe('clampSongPower and execute helpers', function()
    it('clamp floor cap', function()
        assert(e.clampSongPower(65, 95) == 65)
        assert(e.clampSongPower(100, 95) == 95)
        assert(e.clampSongPower(-5, 95) == 0)
        assert(e.clampSongPower(16.5, 25) == 16)
    end)

    it('finale macc requiem tick messages', function()
        assert(e.finaleBonusMacc(0) == 175)
        assert(e.finaleBonusMacc(3) == 190)
        assert(e.requiemTickFor(xi.effect.REQUIEM) == 3)
        assert(e.requiemTickFor(xi.effect.ELEGY) == 0)
        assert(e.successMessageFromParams({ skillchainCount = 1 }) == 268)
        assert(e.successMessageFromParams({ spellEffect = xi.effect.SLEEP_I, skillchainCount = 0 }) == 236)
        assert(e.successMessageFromParams({ spellEffect = xi.effect.ELEGY, skillchainCount = 0 }) == 237)
        assert(e.virelaiMessage(true) == 237)
        assert(e.virelaiMessage(false) == 236)
    end)
end)
