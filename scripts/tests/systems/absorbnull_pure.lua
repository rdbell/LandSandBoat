-----------------------------------
-- Pure system tests for absorb/null dual-wire helpers (slice 6709).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/absorbnull (0864).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local el = xi.element

describe('absorbNullChanceLua', function()
    it('uses roll <= chance', function()
        assert(dmg.absorbNullChanceLua(1, 1))
        assert(not dmg.absorbNullChanceLua(1, 0))
        assert(dmg.absorbNullChanceLua(50, 50))
        assert(not dmg.absorbNullChanceLua(51, 50))
        assert(dmg.absorbNullChanceLua(100, 100))
    end)
end)

describe('calculateAbsorptionFromParams', function()
    it('returns Liement factor when active', function()
        assert(dmg.calculateAbsorptionFromParams({ liementFactor = -1.15 }) == -1.15)
        assert(dmg.calculateAbsorptionFromParams({ liementFactor = 1 }) == 1)
    end)

    it('absorb-all returns -1', function()
        assert(dmg.calculateAbsorptionFromParams({
            liementFactor = 1, absorbAllProc = true,
        }) == -1)
    end)

    it('magic absorb gated by isMagic', function()
        assert(dmg.calculateAbsorptionFromParams({
            liementFactor = 1, absorbMagicProc = true, isMagic = false,
        }) == 1)
        assert(dmg.calculateAbsorptionFromParams({
            liementFactor = 1, absorbMagicProc = true, isMagic = true,
        }) == -1)
    end)

    it('element absorb gated by element > 0', function()
        assert(dmg.calculateAbsorptionFromParams({
            liementFactor = 1, absorbElementProc = true, element = 0,
        }) == 1)
        assert(dmg.calculateAbsorptionFromParams({
            liementFactor = 1, absorbElementProc = true, element = el.WATER,
        }) == -1)
    end)

    it('no absorb returns 1', function()
        assert(dmg.calculateAbsorptionFromParams({
            liementFactor = 1, isMagic = true, element = el.FIRE,
        }) == 1)
    end)

    it('defaults missing liementFactor to 1', function()
        assert(dmg.calculateAbsorptionFromParams({}) == 1)
    end)
end)

describe('calculateNullificationFromParams', function()
    it('empty is pass factor 1', function()
        assert(dmg.calculateNullificationFromParams({}) == 1)
    end)

    it('null-all returns 0', function()
        assert(dmg.calculateNullificationFromParams({ nullAllProc = true }) == 0)
    end)

    it('magic null gated by isMagic', function()
        assert(dmg.calculateNullificationFromParams({
            nullMagicProc = true, isMagic = false,
        }) == 1)
        assert(dmg.calculateNullificationFromParams({
            nullMagicProc = true, isMagic = true,
        }) == 0)
    end)

    it('breath null gated by isBreath', function()
        assert(dmg.calculateNullificationFromParams({
            nullBreathProc = true, isBreath = false,
        }) == 1)
        assert(dmg.calculateNullificationFromParams({
            nullBreathProc = true, isBreath = true,
        }) == 0)
    end)

    it('element null gated by element > 0', function()
        assert(dmg.calculateNullificationFromParams({
            nullElementProc = true, element = 0,
        }) == 1)
        assert(dmg.calculateNullificationFromParams({
            nullElementProc = true, element = el.DARK,
        }) == 0)
    end)
end)
