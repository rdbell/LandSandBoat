-----------------------------------
-- Pure system tests for Rune Fencer dual-wire (slice 6751).
-- Calls production xi.job_utils.rune_fencer pure exports.
-- Goldens match internal/runefencer (0882 / 5978).
-----------------------------------

require('scripts/globals/job_utils/rune_fencer')

local r = xi.job_utils.rune_fencer

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Rune Fencer pure pins', function()
    it('bases', function()
        assert(r.runeDurationSeconds == 300)
        assert(r.swordplayBasePower == 3 and r.swordplayTickPower == 3)
        assert(r.swordplayBaseCap == 60 and r.swordplayDurationSeconds == 120)
        assert(r.swordplayTickSeconds == 3)
        assert(r.vallationBaseSDTPercent == 15)
        assert(r.battutaBaseInquartata == 36 and r.battutaBaseSpikes == 6)
        assert(r.battutaDurationSeconds == 90)
        assert(r.vivaciousPulseBaseHP == 10)
        assert(r.emboldenDurationSec == 60)
        assert(r.elementalSforzoPower == 1 and r.elementalSforzoDurationSec == 30)
        assert(r.oneHourRecastSecondsPerMod == 60)
        assert(r.liementBaseAbsorbPercent == 85)
    end)
end)

describe('runes power pulse swordplay', function()
    it('products', function()
        assert(r.maxRunesFromParams({ runLevel = 1 }) == 1)
        assert(r.maxRunesFromParams({ runLevel = 34 }) == 1)
        assert(r.maxRunesFromParams({ runLevel = 35 }) == 2)
        assert(r.maxRunesFromParams({ runLevel = 64 }) == 2)
        assert(r.maxRunesFromParams({ runLevel = 65 }) == 3)
        assert(r.maxRunesFromParams({ runLevel = 99 }) == 3)

        assert(r.runeEnhancementPowerFromParams({ runLevel = 99, meritBonus = 0, jobPointBonus = 0 }) == 54)
        assert(r.runeEnhancementPowerFromParams({ runLevel = 1, meritBonus = 0, jobPointBonus = 0 }) == 5)
        assert(r.runeEnhancementPowerFromParams({ runLevel = 50, meritBonus = 0, jobPointBonus = 0 }) == 30)
        assert(r.runeEnhancementPowerFromParams({ runLevel = 99, meritBonus = 10, jobPointBonus = 20 }) == 84)

        assert(r.runeHealAmountFromParams({ effectType = xi.effect.IGNIS, stat = 100 }) == 50)
        assert(r.runeHealAmountFromParams({ effectType = xi.effect.IGNIS, stat = 99 }) == 49)
        assert(r.runeHealAmountFromParams({ effectType = xi.effect.TENEBRAE, stat = 200 }) == 0)
        assert(r.runeHealAmountFromParams({ effectType = 0, stat = 100 }) == 0)

        local hp, mp = r.vivaciousPulseFromParams({ maxHP = 1000 })
        assert(hp == 10 and mp == 0)
        hp, mp = r.vivaciousPulseFromParams({ divineSkill = 200, maxHP = 1000 })
        assert(hp == 110)
        hp, mp = r.vivaciousPulseFromParams({
            divineSkill = 200, vivaciousPulseJP = 20, maxHP = 1000,
        })
        assert(hp == 130)
        hp, mp = r.vivaciousPulseFromParams({
            divineSkill = 200, vivaciousPulseJP = 20, runeStatHeals = 40,
            potencyMod = 50, maxHP = 1000,
        })
        assert(hp == 255)
        hp, mp = r.vivaciousPulseFromParams({
            divineSkill = 200, tenebraeCount = 2, maxHP = 1000,
        })
        assert(mp == 60)
        hp, mp = r.vivaciousPulseFromParams({
            divineSkill = 200, currentHP = 990, maxHP = 1000,
        })
        assert(hp == 10)
        hp, mp = r.vivaciousPulseFromParams({
            divineSkill = 200, currentHP = 1000, maxHP = 1000,
        })
        assert(hp == 0)

        assert(r.swordplayPowerFromParams({ swordplayMod = 0 }) == 3)
        assert(r.swordplayPowerFromParams({ swordplayMod = 2 }) == 9)
        assert(almost(r.swordplaySubPowerFromParams({ sleightMerit = 25, augmentsMod = 0 }), 25))
        assert(almost(r.swordplaySubPowerFromParams({ sleightMerit = 25, augmentsMod = 2 }), 35))
        assert(r.swordplayTickDeltaFromParams({ currentPower = 0, swordplayJP = 0 }) == 3)
        assert(r.swordplayTickDeltaFromParams({ currentPower = 58, swordplayJP = 0 }) == 2)
        assert(r.swordplayTickDeltaFromParams({ currentPower = 60, swordplayJP = 0 }) == 0)
        assert(r.swordplayTickDeltaFromParams({ currentPower = 63, swordplayJP = 5 }) == 2)
        assert(r.swordplayTickDeltaFromParams({ currentPower = 65, swordplayJP = 5 }) == 0)
    end)
end)

describe('vallation battuta swipe embolden', function()
    it('products', function()
        assert(r.vallationSDTPowerFromParams({ vallationMerit = 0 }) == 1500)
        assert(r.vallationSDTPowerFromParams({ vallationMerit = 10 }) == 2500)
        assert(almost(r.inspirationFastCastFromParams({
            inspirationMerits = 50, enhancesInspirationMod = 0,
        }), 50))
        assert(almost(r.inspirationFastCastFromParams({
            inspirationMerits = 50, enhancesInspirationMod = 2,
        }), 60))

        local inq, spikes = r.battutaFromParams({ meritPower = 0, enhancesBattuta = 0, runeCount = 1 })
        assert(inq == 36 and spikes == 6)
        inq, spikes = r.battutaFromParams({ meritPower = 20, enhancesBattuta = 0, runeCount = 3 })
        assert(inq == 56 and spikes == 78)
        inq, spikes = r.battutaFromParams({ meritPower = 20, enhancesBattuta = 4, runeCount = 2 })
        assert(spikes == 62)

        assert(almost(r.swipeLungeSkillModifierFromParams({
            weaponSkill = 300, iLvlSkill = 0, swipeJP = 0,
        }), 300))
        assert(almost(r.swipeLungeSkillModifierFromParams({
            weaponSkill = 300, iLvlSkill = 20, swipeJP = 10,
        }), 352))
        assert(r.swipeLungeBaseDamageFromParams({
            skillModifier = 300, numHits = 1, gearBonus = 0, magicDamage = 0,
        }) == 225)
        assert(r.swipeLungeBaseDamageFromParams({
            skillModifier = 300, numHits = 3, gearBonus = 0, magicDamage = 0,
        }) == 375)
        assert(r.swipeLungeBaseDamageFromParams({
            skillModifier = 300, numHits = 1, gearBonus = 20, magicDamage = 10,
        }) == 295)

        local p = r.emboldenFromParams()
        assert(p.duration == 60)
        p = r.elementalSforzoFromParams()
        assert(p.power == 1 and p.duration == 30)
        assert(r.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(r.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)
    end)
end)
