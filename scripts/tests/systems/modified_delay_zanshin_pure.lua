-----------------------------------
-- Pure system tests for modified-delay / Zanshin dual-wire (slice 6680).
-- Calls production xi.combat.tp.getModifiedDelayAndCanZanshinFromParams.
-- Goldens match internal/attackutils GetModifiedDelayAndCanZanshin (0802).
-----------------------------------

require('scripts/globals/combat/tp')

local actor = xi.combat.tp.modifiedDelayActor

local function md(params)
    return xi.combat.tp.getModifiedDelayAndCanZanshinFromParams(params)
end

describe('Modified delay constants', function()
    it('pins H2H floors and DELAYP mult floor', function()
        assert(xi.combat.tp.h2hSingleSwingMinDelay == 96)
        assert(xi.combat.tp.h2hFistMinDelay == 48)
        assert(xi.combat.tp.delayPMinMultiplier == 0.85)
        assert(actor.PC == 0 and actor.MOB == 1 and actor.OTHER == 2)
    end)
end)

describe('Modified delay single swing', function()
    it('sets canZanshin and leaves delay without DELAYP', function()
        local got = md({ delay = 480 })
        assert(got.modifiedDelay == 480 and got.canZanshin)

        got = md({ delay = 0 })
        assert(got.modifiedDelay == 0 and got.canZanshin)
    end)
end)

describe('Modified delay dual wield', function()
    it('halves delay for single-hit TP return and never Zanshin', function()
        local got = md({ delay = 480, dualWield = true })
        assert(got.modifiedDelay == 240 and not got.canZanshin)

        -- DW 25%: (480 * 75/100) / 2 = 180
        got = md({ delay = 480, dualWield = true, dualWieldMod = 25 })
        assert(got.modifiedDelay == 180 and not got.canZanshin)

        -- Fractional half then floor: (481 * 100/100) / 2 = 240.5 → 240
        got = md({ delay = 481, dualWield = true })
        assert(got.modifiedDelay == 240)

        -- DW wins over H2H (if/elseif); canZanshin stays false
        got = md({
            delay = 480,
            dualWield = true,
            usingH2H = true,
            actorKind = actor.PC,
            h2hSkillRankZero = true,
        })
        assert(got.modifiedDelay == 240 and not got.canZanshin)
    end)
end)

describe('Modified delay H2H PC', function()
    it('multi-fist halves after Martial Arts with 48 floor; no Zanshin', function()
        local got = md({ delay = 480, usingH2H = true, actorKind = actor.PC })
        assert(got.modifiedDelay == 240 and not got.canZanshin)

        got = md({ delay = 480, usingH2H = true, actorKind = actor.PC, martialArtsMod = 80 })
        assert(got.modifiedDelay == 200 and not got.canZanshin)

        got = md({ delay = 40, usingH2H = true, actorKind = actor.PC })
        assert(got.modifiedDelay == 48 and not got.canZanshin)
    end)

    it('shield or zero rank uses single-swing floor 96 and allows Zanshin', function()
        local got = md({
            delay = 480,
            usingH2H = true,
            actorKind = actor.PC,
            subEquipped = true,
            martialArtsMod = 80,
        })
        assert(got.modifiedDelay == 400 and got.canZanshin)

        got = md({
            delay = 480,
            usingH2H = true,
            actorKind = actor.PC,
            h2hSkillRankZero = true,
        })
        assert(got.modifiedDelay == 480 and got.canZanshin)

        got = md({
            delay = 50,
            usingH2H = true,
            actorKind = actor.PC,
            h2hSkillRankZero = true,
        })
        assert(got.modifiedDelay == 96 and got.canZanshin)

        -- Large MA still floors at 96
        got = md({
            delay = 96,
            usingH2H = true,
            actorKind = actor.PC,
            subEquipped = true,
            martialArtsMod = 100,
        })
        assert(got.modifiedDelay == 96 and got.canZanshin)
    end)
end)

describe('Modified delay H2H mob and other', function()
    it('mob halves delay, ignores Martial Arts, no Zanshin', function()
        local got = md({
            delay = 480,
            usingH2H = true,
            actorKind = actor.MOB,
            martialArtsMod = 80,
        })
        assert(got.modifiedDelay == 240 and not got.canZanshin)

        got = md({ delay = 40, usingH2H = true, actorKind = actor.MOB })
        assert(got.modifiedDelay == 48)
    end)

    it('other (pet/trust) uses (delay-MA)/2 with 48 floor', function()
        local got = md({
            delay = 480,
            usingH2H = true,
            actorKind = actor.OTHER,
            martialArtsMod = 40,
        })
        assert(got.modifiedDelay == 220 and not got.canZanshin)

        got = md({ delay = 40, usingH2H = true, actorKind = actor.OTHER })
        assert(got.modifiedDelay == 48)
    end)
end)

describe('Modified delay DELAYP', function()
    it('scales after DW/H2H with -15% mult floor', function()
        local got = md({ delay = 480, delayP = 0 })
        assert(got.modifiedDelay == 480)

        got = md({ delay = 480, delayP = 10 })
        assert(got.modifiedDelay == 528 and got.canZanshin)

        got = md({ delay = 480, delayP = -15 })
        assert(got.modifiedDelay == 408)

        -- -20 still floors mult at 0.85
        got = md({ delay = 480, delayP = -20 })
        assert(got.modifiedDelay == 408)

        -- DW + DELAYP: 240.5 * 0.85 = 204.425 → 204
        got = md({ delay = 481, dualWield = true, delayP = -15 })
        assert(got.modifiedDelay == 204 and not got.canZanshin)

        -- H2H multi-fist then DELAYP: 200 * 1.10 = 220
        got = md({
            delay = 480,
            usingH2H = true,
            actorKind = actor.PC,
            martialArtsMod = 80,
            delayP = 10,
        })
        assert(got.modifiedDelay == 220 and not got.canZanshin)
    end)
end)

describe('Modified delay table samples', function()
    it('matches Go table-driven pins', function()
        local got = md({ delay = 450 })
        assert(got.modifiedDelay == 450 and got.canZanshin)

        -- DW 30%: (360 * 70/100) / 2 = 126
        got = md({ delay = 360, dualWield = true, dualWieldMod = 30 })
        assert(got.modifiedDelay == 126 and not got.canZanshin)
    end)
end)
