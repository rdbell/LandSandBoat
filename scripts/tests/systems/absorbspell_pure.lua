-----------------------------------
-- Pure system tests for absorb-spell dual-wire (slice 6723).
-- Calls production xi.spells.absorb pure exports.
-- Goldens match internal/absorbspell (0880).
-----------------------------------

require('scripts/globals/spells/absorb_spell')

local a = xi.spells.absorb

local function almost(x, y)
    return math.abs(x - y) < 1e-9
end

describe('Absorb spell pure pins', function()
    it('clamps and softcaps', function()
        assert(a.statDurationMin == 0)
        assert(a.statDurationMax == 10000)
        assert(a.absorbTPDamageCap == 3000)
        assert(a.skillSoftcap == 300)
        assert(a.maxHPBoostBaseDuration == 180)
    end)
end)

describe('absorbStatPotencyFromParams', function()
    it('base level ladder and gear floors', function()
        -- base = 3 + floor(lvl/5); lvl 99 → 3+19 = 22
        assert(a.absorbStatPotencyFromParams({ mainLevel = 99 }) == 22)
        assert(a.absorbStatPotencyFromParams({ mainLevel = 0 }) == 3)
        assert(a.absorbStatPotencyFromParams({ mainLevel = 4 }) == 3)
        assert(a.absorbStatPotencyFromParams({ mainLevel = 5 }) == 4)

        -- floor(floor(22 * 1.1 * 1.2) * 1.5) with NV
        local p = a.absorbStatPotencyFromParams({
            mainLevel = 99, augmentsAbsorb = 10, liberator = 20,
            hasNetherVoid = true, netherVoidPower = 50,
        })
        assert(p == math.floor(math.floor(22 * 1.1 * 1.2) * 1.5))

        -- NV inactive ignores power
        assert(a.absorbStatPotencyFromParams({
            mainLevel = 99, hasNetherVoid = false, netherVoidPower = 50,
        }) == 22)
    end)
end)

describe('absorbStatDurationFromParams', function()
    it('skill base clamp and multipliers', function()
        -- skill 0: floor((0-490.5)/5)=floor(-98.1)=-99 → 81
        assert(a.absorbStatDurationFromParams({ darkSkill = 0 }) == 81)
        -- skill 490: floor(-0.1)=-1 → 179
        assert(a.absorbStatDurationFromParams({ darkSkill = 490 }) == 179)
        -- skill 491: floor(0.1)=0 → 180
        assert(a.absorbStatDurationFromParams({ darkSkill = 491 }) == 180)

        local d = a.absorbStatDurationFromParams({
            darkSkill = 500, darkMagicDuration = 10, absorbEffectDuration = 20, enhancesAbsorb = 5,
        })
        local base = utils.clamp(180 + math.floor((500 - 490.5) / 5), 0, 10000)
        assert(d == math.floor(base * 1.1 * 1.2) + 5)
    end)
end)

describe('drainAspirRangeFromParams', function()
    it('low and high skill bands', function()
        local minP, maxP = a.drainAspirRangeFromParams({
            spellId = xi.magic.spell.DRAIN, darkSkill = 300,
        })
        -- low: floor(300*1+20)=320; min floor(320*0.5)=160
        assert(maxP == 320 and minP == 160)

        minP, maxP = a.drainAspirRangeFromParams({
            spellId = xi.magic.spell.DRAIN, darkSkill = 301,
        })
        -- high: floor(301*0.625+132.5)
        assert(maxP == math.floor(301 * 0.625 + 132.5))
        assert(minP == math.floor(maxP * 0.50))

        minP, maxP = a.drainAspirRangeFromParams({
            spellId = xi.magic.spell.ASPIR, darkSkill = 100,
        })
        assert(maxP == math.floor(100 * 0.3 + 20))
        assert(minP == math.floor(maxP * 0.50))

        local u1, u2 = a.drainAspirRangeFromParams({ spellId = 0, darkSkill = 100 })
        assert(u1 == nil and u2 == nil)
    end)
end)

describe('drainAspirProductFromParams', function()
    it('floor chain product', function()
        local d = a.drainAspirProductFromParams({
            baseDamage = 100,
            resistTier = 1, additionalResistTier = 1, sdt = 1,
            elementalStaffBonus = 1, elementalAffinity = 1, dayAndWeather = 1,
            augmentsAbsorb = 0, enhDrainAspir = 0, liberator = 0,
        })
        assert(d == 100)

        d = a.drainAspirProductFromParams({
            baseDamage = 100,
            resistTier = 0.5, additionalResistTier = 1, sdt = 1,
            elementalStaffBonus = 1.1, elementalAffinity = 1, dayAndWeather = 1,
            augmentsAbsorb = 10, enhDrainAspir = 5, liberator = 0,
            hasNetherVoid = true, netherVoidPower = 50,
        })
        local expect = 100
        expect = math.floor(expect * 0.5)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1.1)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * (1 + 0.10 + 0.05))
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1.5)
        assert(d == expect)
    end)
end)

describe('maxHPBoostDurationFromParams', function()
    it('180 + 180*dark%', function()
        assert(almost(a.maxHPBoostDurationFromParams({ darkMagicDuration = 0 }), 180))
        assert(almost(a.maxHPBoostDurationFromParams({ darkMagicDuration = 50 }), 270))
        assert(almost(a.maxHPBoostDurationFromParams({ darkMagicDuration = 100 }), 360))
    end)
end)

describe('absorbTPDamageFromParams', function()
    it('base 30% TP product and clamp', function()
        local d = a.absorbTPDamageFromParams({
            targetTP = 1000,
            resistTier = 1, additionalResistTier = 1, sdt = 1,
            elementalStaffBonus = 1, dayAndWeather = 1,
        })
        assert(d == 300)

        d = a.absorbTPDamageFromParams({
            targetTP = 10000,
            resistTier = 1, additionalResistTier = 1, sdt = 1,
            elementalStaffBonus = 1, dayAndWeather = 1,
            augmentsAbsorb = 0, augmentsAbsorbTP = 0, liberator = 0,
        })
        assert(d == 3000) -- clamp

        d = a.absorbTPDamageFromParams({
            targetTP = 1000,
            resistTier = 0.5, additionalResistTier = 1, sdt = 1,
            elementalStaffBonus = 1, dayAndWeather = 1,
            augmentsAbsorb = 10, augmentsAbsorbTP = 20, liberator = 0,
        })
        local expect = 1000 * 0.3
        expect = math.floor(expect * 0.5)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1)
        expect = math.floor(expect * 1.1)
        expect = math.floor(expect * 1.2)
        expect = math.floor(expect * 1)
        assert(d == expect)
    end)
end)
