-----------------------------------
-- Pure system tests for Magic Critical Hit II dual-wire (slice 6714).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/magicbonus CriticalMultiplier (0858).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Magic Critical Hit II constants', function()
    it('pins 1.25 proc multiplier', function()
        assert(almost(dmg.magicCriticalIIMultiplier, 1.25))
    end)
end)

describe('calculateMagicCriticalMultiplierFromParams', function()
    it('returns 1 on empty or invalid roll', function()
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({}), 1.0))
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({
            critChanceII = 100, critIIRoll = 0,
        }), 1.0))
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({
            critChanceII = 100, critIIRoll = 101,
        }), 1.0))
    end)

    it('procs when roll in 1..100 and roll <= chance', function()
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({
            critChanceII = 49, critIIRoll = 50,
        }), 1.0))
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({
            critChanceII = 50, critIIRoll = 50,
        }), 1.25))
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({
            critChanceII = 100, critIIRoll = 1,
        }), 1.25))
        assert(almost(dmg.calculateMagicCriticalMultiplierFromParams({
            critChanceII = 0, critIIRoll = 1,
        }), 1.0))
    end)
end)
