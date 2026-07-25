-----------------------------------
-- Pure system tests for Cardinal Chant dual-wire (slice 6716).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/cardinalchant (0860).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local dir = xi.direction
local skill = xi.skill
local spell = xi.magic.spell

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Cardinal Chant constants and IsRaSpell', function()
    it('pins fervor and trait bounds', function()
        assert(almost(dmg.cardinalChantFervorFactor, 1.5))
        assert(dmg.cardinalChantTraitMin == 0)
        assert(dmg.cardinalChantTraitMax == 4)
    end)

    it('ra spell membership', function()
        assert(dmg.cardinalChantIsRaSpell(spell.FIRA))
        assert(dmg.cardinalChantIsRaSpell(spell.WATERA_III))
        assert(dmg.cardinalChantIsRaSpell(spell.THUNDARA_II))
        assert(not dmg.cardinalChantIsRaSpell(spell.FLARE))
        assert(not dmg.cardinalChantIsRaSpell(0))
        assert(not dmg.cardinalChantIsRaSpell(144)) -- Fire
    end)
end)

describe('cardinalChantBaseBonus', function()
    it('trait table and clamps', function()
        assert(dmg.cardinalChantBaseBonus(0, dir.EAST, false) == 0)
        assert(dmg.cardinalChantBaseBonus(1, dir.EAST, false) == 5)
        assert(dmg.cardinalChantBaseBonus(1, dir.WEST, true) == 15)
        assert(dmg.cardinalChantBaseBonus(4, dir.WEST, true) == 28)
        assert(dmg.cardinalChantBaseBonus(4, dir.NORTH, false) == 13)
        assert(dmg.cardinalChantBaseBonus(-3, dir.EAST, false) == 0)
        assert(dmg.cardinalChantBaseBonus(99, dir.WEST, true) == 28)
        -- unknown direction
        assert(dmg.cardinalChantBaseBonus(4, 1, true) == 0)
    end)
end)

describe('cardinalChantAngleFactor', function()
    it('EAST wedge around 0', function()
        assert(dmg.cardinalChantAngleFactor(dir.EAST, 0) == 1)
        assert(almost(dmg.cardinalChantAngleFactor(dir.EAST, 32), 0.5))
        assert(dmg.cardinalChantAngleFactor(dir.EAST, 64) == 0)
        assert(almost(dmg.cardinalChantAngleFactor(dir.EAST, 224), 0.5))
        assert(dmg.cardinalChantAngleFactor(dir.EAST, 192) == 0)
        assert(almost(dmg.cardinalChantAngleFactor(dir.EAST, 255), 1 - 1 / 64))
    end)

    it('SOUTH WEST NORTH optima', function()
        assert(dmg.cardinalChantAngleFactor(dir.SOUTH, 64) == 1)
        assert(almost(dmg.cardinalChantAngleFactor(dir.SOUTH, 32), 0.5))
        assert(dmg.cardinalChantAngleFactor(dir.SOUTH, 0) == 0)
        assert(dmg.cardinalChantAngleFactor(dir.WEST, 128) == 1)
        assert(almost(dmg.cardinalChantAngleFactor(dir.WEST, 160), 0.5))
        assert(dmg.cardinalChantAngleFactor(dir.NORTH, 192) == 1)
        assert(almost(dmg.cardinalChantAngleFactor(dir.NORTH, 224), 0.5))
        assert(dmg.cardinalChantAngleFactor(1, 0) == 0)
    end)
end)

describe('cardinalChantBonusFromParams', function()
    it('early returns', function()
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 0, skillType = skill.ELEMENTAL_MAGIC, direction = dir.EAST,
            traitLevel = 4, worldRotation = 0,
        }) == 0)
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 144, skillType = skill.NINJUTSU, direction = dir.EAST,
            traitLevel = 4, worldRotation = 0,
        }) == 0)
    end)

    it('optimal non-ra and ra', function()
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 144, skillType = skill.ELEMENTAL_MAGIC, direction = dir.EAST,
            traitLevel = 1, worldRotation = 0,
        }) == 5)
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 144, skillType = skill.ELEMENTAL_MAGIC, direction = dir.WEST,
            traitLevel = 4, worldRotation = 128,
        }) == 22)
        assert(dmg.cardinalChantBonusFromParams({
            spellId = spell.FIRA, skillType = skill.ELEMENTAL_MAGIC, direction = dir.WEST,
            traitLevel = 1, worldRotation = 128,
        }) == 15)
        assert(dmg.cardinalChantBonusFromParams({
            spellId = spell.WATERA_III, skillType = skill.ELEMENTAL_MAGIC, direction = dir.WEST,
            traitLevel = 4, worldRotation = 128,
        }) == 28)
    end)

    it('fervor gear and angle scale', function()
        -- base 5 * 1.5 = 7.5 → floor 7
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 144, skillType = skill.ELEMENTAL_MAGIC, direction = dir.EAST,
            traitLevel = 1, hasCollimatedFervor = true, worldRotation = 0,
        }) == 7)
        -- base 5 * 1.2 = 6
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 144, skillType = skill.ELEMENTAL_MAGIC, direction = dir.EAST,
            traitLevel = 1, gearBonusPercent = 20, worldRotation = 0,
        }) == 6)
        -- base 5 * 0.5 angle = 2.5 → floor 2
        assert(dmg.cardinalChantBonusFromParams({
            spellId = 144, skillType = skill.ELEMENTAL_MAGIC, direction = dir.EAST,
            traitLevel = 1, worldRotation = 32,
        }) == 2)
    end)
end)
