-----------------------------------
-- Pure system tests for damage_spell magic burst mult dual-wire (slice 6715).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/magicburst (0853).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local el = xi.element
local spell = xi.magic.spell

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Magic burst dmg constants and rank bonus', function()
    it('pins base and capped max', function()
        assert(almost(dmg.magicBurstBase, 1.25))
        assert(almost(dmg.magicBurstCappedBonusMax, 0.4))
        assert(almost(dmg.magicBurstRankBonusLow, 1.5))
        assert(almost(dmg.magicBurstRankBonusHigh, 0.0))
    end)

    it('rank ladder extremes and mid table', function()
        assert(almost(dmg.magicBurstRankBonus(-100), 1.5))
        assert(almost(dmg.magicBurstRankBonus(-3), 1.5))
        assert(almost(dmg.magicBurstRankBonus(-2), 1.15))
        assert(almost(dmg.magicBurstRankBonus(-1), 0.85))
        assert(almost(dmg.magicBurstRankBonus(0), 0.6))
        assert(almost(dmg.magicBurstRankBonus(1), 0.5))
        assert(almost(dmg.magicBurstRankBonus(2), 0.4))
        assert(almost(dmg.magicBurstRankBonus(3), 0.15))
        assert(almost(dmg.magicBurstRankBonus(4), 0.05))
        assert(almost(dmg.magicBurstRankBonus(5), 0.0))
        assert(almost(dmg.magicBurstRankBonus(11), 0.0))
    end)
end)

describe('calculateIfMagicBurstFromParams', function()
    it('NONE element keeps base 1 without rank/SC', function()
        local m, c = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.NONE, skillchainCount = 3, resistRank = -3,
        })
        assert(almost(m, 1.0) and not c)
    end)

    it('rank and skillchain count assembly', function()
        -- rank 0 → 0.6; SC 2 → +0.2; total 1.25+0.6+0.2=2.05
        local m = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.FIRE, skillchainCount = 2, resistRank = 0,
        })
        assert(almost(m, 2.05))
        -- rank -3 → 1.5; SC 0 → 2.75
        m = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.ICE, skillchainCount = 0, resistRank = -3,
        })
        assert(almost(m, 2.75))
        -- rank 5 → 0; SC 5 → 1.75
        m = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.THUNDER, skillchainCount = 5, resistRank = 5,
        })
        assert(almost(m, 1.75))
        -- rank 4 → 0.05; SC 1 → 1.4
        m = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.DARK, skillchainCount = 1, resistRank = 4,
        })
        assert(almost(m, 1.4))
    end)

    it('Sengikori add and consume gate', function()
        local m, c = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.FIRE, skillchainCount = 1, resistRank = 5, sengikoriMod = 25,
        })
        -- 1.25+0+0.1+0.25=1.6
        assert(almost(m, 1.6) and c)
        m, c = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.FIRE, skillchainCount = 0, resistRank = 5, sengikoriMod = 25,
        })
        assert(almost(m, 1.25) and not c)
        m, c = dmg.calculateIfMagicBurstFromParams({
            spellElement = el.NONE, skillchainCount = 1, sengikoriMod = 10,
        })
        assert(almost(m, 1.1) and c)
    end)
end)

describe('calculateIfMagicBurstBonusFromParams', function()
    it('neutral and capped/uncapped', function()
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({}), 1.0))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            cappedBonusMod = 20, uncappedBonusMod = 10,
        }), 1.30))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            cappedBonusMod = -50,
        }), 1.0))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            cappedBonusMod = 50,
        }), 1.4))
    end)

    it('Ancient Magic merit band', function()
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            spellId = spell.FLARE, cappedBonusMod = 10, ancientMagicMerit = 15,
        }), 1.25))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            spellId = 100, cappedBonusMod = 10, ancientMagicMerit = 15,
        }), 1.10))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            spellId = spell.FLOOD_II, ancientMagicMerit = 50,
        }), 1.4))
    end)

    it('Innin JP and cardinal chant uncapped', function()
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            inninMerit = 10, magicBurstJP = 5, cardinalChantWest = 3,
        }), 1.18))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            cappedBonusMod = 30, inninMerit = 20, magicBurstJP = 10,
        }), 1.5))
        assert(almost(dmg.calculateIfMagicBurstBonusFromParams({
            spellId = spell.FLOOD_II, cappedBonusMod = 25, uncappedBonusMod = 5,
            ancientMagicMerit = 10, inninMerit = 10, magicBurstJP = 8, cardinalChantWest = 2,
        }), 1.55))
    end)
end)
