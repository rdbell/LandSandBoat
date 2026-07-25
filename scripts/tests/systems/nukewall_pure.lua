-----------------------------------
-- Pure system tests for Nuke Wall dual-wire helpers (slice 6708).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/nukewall (0863).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local el = xi.element

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Nuke Wall constants and damageCap', function()
    it('pins potency and decay steps', function()
        assert(dmg.nukeWallPotencyMin == 0)
        assert(dmg.nukeWallPotencyMax == 4000)
        assert(dmg.nukeWallPotencyDecay == 2000)
        assert(dmg.nukeWallDecayRemainingMs == 4000)
        assert(dmg.nukeWallDurationSec == 5)
        assert(dmg.nukeWallLevelDamageScale == 21)
        assert(dmg.nukeWallLevelDamageBase == 500)
    end)

    it('damageCap = lvl*21 + 500', function()
        assert(dmg.nukeWallDamageCap(0) == 500)
        assert(dmg.nukeWallDamageCap(75) == 75 * 21 + 500)
        assert(dmg.nukeWallDamageCap(99) == 99 * 21 + 500)
    end)
end)

describe('nukeWallApplyTimeDecay', function()
    it('decays only when remaining <= 4000', function()
        assert(dmg.nukeWallApplyTimeDecay(4000, 4000) == 2000)
        assert(dmg.nukeWallApplyTimeDecay(1500, 1000) == 0)
        assert(dmg.nukeWallApplyTimeDecay(3000, 4001) == 3000)
    end)
end)

describe('nukeWallRaykeMatchesElement and ApplyRayke', function()
    it('matches packed nibbles and halves', function()
        -- pack fire in low nibble
        local sub = el.FIRE
        assert(dmg.nukeWallRaykeMatchesElement(el.FIRE, sub))
        assert(not dmg.nukeWallRaykeMatchesElement(el.ICE, sub))
        assert(dmg.nukeWallApplyRayke(3000, el.FIRE, true, sub) == 1500)
        assert(dmg.nukeWallApplyRayke(3000, el.FIRE, false, sub) == 3000)
        assert(dmg.nukeWallApplyRayke(3000, el.ICE, true, sub) == 3000)
    end)
end)

describe('nukeWallFactorFromPotency and nextPotency', function()
    it('factor is 1 - potency/10000', function()
        assert(almost(dmg.nukeWallFactorFromPotency(0), 1.0))
        assert(almost(dmg.nukeWallFactorFromPotency(2000), 0.8))
        assert(almost(dmg.nukeWallFactorFromPotency(4000), 0.6))
    end)

    it('nextPotency clamps with damage share', function()
        -- L75 cap = 2075; damage 2075 → add 4000
        assert(dmg.nukeWallNextPotency(0, 2075, 75) == 4000)
        assert(dmg.nukeWallNextPotency(1000, 0, 75) == 1000)
        -- half damage → floor(4000*0.5)=2000
        local half = math.floor(2075 / 2)
        local added = math.floor(4000 * half / 2075)
        assert(dmg.nukeWallNextPotency(0, half, 75) == added)
    end)
end)

describe('calculateNukeWallFactorFromParams', function()
    it('early returns factor 1', function()
        local f, n, a, c = dmg.calculateNukeWallFactorFromParams({
            isNM = false, spellElement = el.FIRE, finalDamage = 100, mainLvl = 75,
        })
        assert(f == 1 and n == 0 and not a and not c)

        f, n, a, c = dmg.calculateNukeWallFactorFromParams({
            isNM = true, spellElement = el.NONE, finalDamage = 100, mainLvl = 75,
        })
        assert(f == 1 and not a)

        f, n, a, c = dmg.calculateNukeWallFactorFromParams({
            isNM = true, spellElement = el.FIRE, finalDamage = -1, mainLvl = 75,
        })
        assert(f == 1 and not a)
    end)

    it('no prior wall builds next potency only', function()
        local f, n, a, c = dmg.calculateNukeWallFactorFromParams({
            isNM = true, spellElement = el.FIRE, finalDamage = 2075, mainLvl = 75,
            hasNukeWall = false,
        })
        assert(almost(f, 1.0) and a and not c)
        assert(n == 4000)
    end)

    it('prior wall with decay and rayke', function()
        local f, n, a, c = dmg.calculateNukeWallFactorFromParams({
            isNM = true, spellElement = el.FIRE, finalDamage = 100, mainLvl = 75,
            hasNukeWall = true, wallPotency = 4000, timeRemainingMs = 3000,
            hasRayke = true, raykeSubPower = el.FIRE,
        })
        -- decay 4000-2000=2000; rayke floor(2000/2)=1000; factor 0.9
        assert(almost(f, 0.9) and a and c)
        local nextWant = dmg.nukeWallNextPotency(1000, 100, 75)
        assert(n == nextWant)
    end)

    it('prior wall no decay keeps full potency for factor', function()
        local f = dmg.calculateNukeWallFactorFromParams({
            isNM = true, spellElement = el.ICE, finalDamage = 50, mainLvl = 99,
            hasNukeWall = true, wallPotency = 2000, timeRemainingMs = 5000,
        })
        assert(almost(f, 0.8))
    end)
end)
