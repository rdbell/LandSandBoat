-----------------------------------
-- Pure system tests for magic hit rate dual-wire helpers (slice 6678).
-- Calls production xi.combat.magicHitRate pure exports (not local copies).
-----------------------------------

require('scripts/globals/combat/magic_hit_rate')

describe('Magic hit rate clampResistRank', function()
    it('clamps to [-3, 11]', function()
        assert(xi.combat.magicHitRate.clampResistRank(-5) == -3)
        assert(xi.combat.magicHitRate.clampResistRank(20) == 11)
        assert(xi.combat.magicHitRate.clampResistRank(0) == 0)
        assert(xi.combat.magicHitRate.clampResistRank(7) == 7)
    end)
end)

describe('Magic hit rate resistRankMultiplier', function()
    it('returns golden table values after clamp', function()
        assert(xi.combat.magicHitRate.resistRankMultiplier(-3) == 0.95)
        assert(math.abs(xi.combat.magicHitRate.resistRankMultiplier(-2) - 0.96019) < 1e-12)
        assert(xi.combat.magicHitRate.resistRankMultiplier(0) == 1)
        assert(xi.combat.magicHitRate.resistRankMultiplier(1) == 1.023)
        assert(xi.combat.magicHitRate.resistRankMultiplier(9) == 2.2)
        assert(xi.combat.magicHitRate.resistRankMultiplier(11) == 2.35)
        -- out of range clamps into table
        assert(xi.combat.magicHitRate.resistRankMultiplier(-99) == 0.95)
        assert(xi.combat.magicHitRate.resistRankMultiplier(99) == 2.35)
    end)
end)

describe('Magic hit rate magicAccuracyFromStatDifference', function()
    it('uses mid band identity for |diff| < 11', function()
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(0) == 0)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(10) == 10)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(-10) == -10)
    end)

    it('uses soft bands 11..30 and -11..-30', function()
        assert(math.abs(xi.combat.magicHitRate.magicAccuracyFromStatDifference(11) - 10.5) < 1e-12)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(30) == 20)
        assert(math.abs(xi.combat.magicHitRate.magicAccuracyFromStatDifference(-11) - (-10.5)) < 1e-12)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(-30) == -20)
    end)

    it('uses hard bands and clamps ±30', function()
        assert(math.abs(xi.combat.magicHitRate.magicAccuracyFromStatDifference(31) - 20.25) < 1e-12)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(70) == 30)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(71) == 30)
        assert(math.abs(xi.combat.magicHitRate.magicAccuracyFromStatDifference(-31) - (-20.25)) < 1e-12)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(-70) == -30)
        assert(xi.combat.magicHitRate.magicAccuracyFromStatDifference(-71) == -30)
    end)
end)

describe('Magic hit rate calculateMagicHitRate', function()
    it('maps even MACC/MEVA to 0.50', function()
        assert(xi.combat.magicHitRate.calculateMagicHitRate(300, 300) == 0.50)
    end)

    it('applies positive and cap paths', function()
        assert(math.abs(xi.combat.magicHitRate.calculateMagicHitRate(340, 300) - 0.90) < 1e-12)
        assert(xi.combat.magicHitRate.calculateMagicHitRate(400, 300) == 0.95)
    end)

    it('half-floors negative diffs toward -inf', function()
        -- diff -10 → floor(-5) = -5 → 0.45
        assert(xi.combat.magicHitRate.calculateMagicHitRate(290, 300) == 0.45)
        -- diff -1 → floor(-0.5) = -1 → 0.49
        assert(xi.combat.magicHitRate.calculateMagicHitRate(299, 300) == 0.49)
        -- deep miss floors at 0.05
        assert(xi.combat.magicHitRate.calculateMagicHitRate(100, 300) == 0.05)
    end)
end)

describe('Magic hit rate maxResistTier', function()
    it('gives non-PC always 3', function()
        assert(xi.combat.magicHitRate.maxResistTier(false, -100) == 3)
        assert(xi.combat.magicHitRate.maxResistTier(false, 0) == 3)
    end)

    it('gates PC by elemental MEVA screen', function()
        assert(xi.combat.magicHitRate.maxResistTier(true, -1) == 1)
        assert(xi.combat.magicHitRate.maxResistTier(true, 0) == 2)
        assert(xi.combat.magicHitRate.maxResistTier(true, 1) == 3)
    end)
end)

describe('Magic hit rate resistanceFactorFromTier', function()
    it('maps tier to 1/2^tier', function()
        assert(xi.combat.magicHitRate.resistanceFactorFromTier(0) == 1)
        assert(xi.combat.magicHitRate.resistanceFactorFromTier(1) == 0.5)
        assert(xi.combat.magicHitRate.resistanceFactorFromTier(2) == 0.25)
        assert(xi.combat.magicHitRate.resistanceFactorFromTier(3) == 0.125)
        assert(xi.combat.magicHitRate.resistanceFactorFromTier(-1) == 1)
    end)
end)

describe('Magic hit rate countResistTiers', function()
    it('counts successive resists until a hit', function()
        assert(xi.combat.magicHitRate.countResistTiers(3, { true, true, true }) == 3)
        assert(xi.combat.magicHitRate.countResistTiers(3, { false, true, true }) == 0)
        assert(xi.combat.magicHitRate.countResistTiers(3, { true, true, false }) == 2)
        assert(xi.combat.magicHitRate.countResistTiers(1, { true, true, true }) == 1)
        assert(xi.combat.magicHitRate.countResistTiers(3, { true }) == 1)
        assert(xi.combat.magicHitRate.countResistTiers(3, {}) == 0)
    end)
end)

describe('Magic hit rate autoResistFactor', function()
    it('short-circuits only at rank ≥ 11', function()
        local factor, auto = xi.combat.magicHitRate.autoResistFactor(10, 0)
        assert(not auto)
        assert(factor == 0)

        factor, auto = xi.combat.magicHitRate.autoResistFactor(11, 0)
        assert(auto)
        assert(factor == 0.25)

        factor, auto = xi.combat.magicHitRate.autoResistFactor(11, 42)
        assert(auto)
        assert(factor == 0)

        factor, auto = xi.combat.magicHitRate.autoResistFactor(12, 0)
        assert(auto)
        assert(factor == 0.25)
    end)
end)

describe('Magic hit rate skipHitRateAssembly', function()
    it('is true only for rank ∈ [10, 11)', function()
        assert(not xi.combat.magicHitRate.skipHitRateAssembly(9))
        assert(xi.combat.magicHitRate.skipHitRateAssembly(10))
        assert(not xi.combat.magicHitRate.skipHitRateAssembly(11))
        assert(not xi.combat.magicHitRate.skipHitRateAssembly(0))
    end)
end)

-----------------------------------
-- Actor MACC contribution pure (slice 6705 / internal/magacc)
-- Goldens match Go internal/magacc tests.
-----------------------------------

local mhr = xi.combat.magicHitRate
local skill = xi.skill
local job = xi.job
local el = xi.element
local weather = xi.weather
local effect = xi.effect
local spellGroup = xi.magic.spellGroup

describe('Magic hit rate MACC constants', function()
    it('pins seal / weather / burst / SV steps', function()
        assert(mhr.klimaformBonus == 15)
        assert(mhr.sealBonus == 256)
        assert(mhr.dayWeatherProcChance == 33)
        assert(mhr.singleWeatherAcc == 5)
        assert(mhr.doubleWeatherAcc == 10)
        assert(mhr.dayElementAcc == 5)
        assert(mhr.iridescenceAccStep == 5)
        assert(mhr.magicBurstAccBonus == 100)
        assert(mhr.soulVoiceMultiplier == 2)
        assert(mhr.marcatoMultiplier == 1.5)
    end)
end)

describe('magicAccuracyFromSkillFromParams', function()
    it('passes through known skill level', function()
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.ELEMENTAL_MAGIC, skillLevel = 250,
        }) == 250)
    end)

    it('mob zero skill falls back to A+ cap', function()
        local cap75 = xi.data.skillLevel.getSkillCap(75, xi.skillRank.A_PLUS)
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.ELEMENTAL_MAGIC, skillLevel = 0, mainLvl = 75, isMob = true,
        }) == cap75)
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.ELEMENTAL_MAGIC, skillLevel = 0, mainLvl = 75, isMob = false,
        }) == 0)
    end)

    it('PC singing adds wind full / string floor half', function()
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.SINGING, skillLevel = 100, isPC = true,
            rangeSkillType = skill.WIND_INSTRUMENT, rangeSkillLvl = 50,
        }) == 150)
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.SINGING, skillLevel = 100, isPC = true,
            rangeSkillType = skill.STRING_INSTRUMENT, rangeSkillLvl = 51,
        }) == 125)
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.SINGING, skillLevel = 100, isPC = true,
            rangeSkillType = skill.ELEMENTAL_MAGIC, rangeSkillLvl = 99,
        }) == 100)
    end)

    it('non-PC singing doubles', function()
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillType = skill.SINGING, skillLevel = 80, isPC = false,
        }) == 160)
    end)

    it('skillRank and default A+ caps', function()
        local capB = xi.data.skillLevel.getSkillCap(50, xi.skillRank.B)
        assert(mhr.magicAccuracyFromSkillFromParams({
            skillRank = xi.skillRank.B, mainLvl = 50,
        }) == capB)
        local capA = xi.data.skillLevel.getSkillCap(99, xi.skillRank.A_PLUS)
        assert(mhr.magicAccuracyFromSkillFromParams({ mainLvl = 99 }) == capA)
    end)
end)

describe('magicAccuracyFromElementFromParams', function()
    it('returns 0 for none and macc+staff*10 otherwise', function()
        assert(mhr.magicAccuracyFromElementFromParams({
            magicalElement = el.NONE, elementalMaccMod = 20, elementalStaffMod = 3,
        }) == 0)
        assert(mhr.magicAccuracyFromElementFromParams({
            magicalElement = el.FIRE, elementalMaccMod = 15, elementalStaffMod = 2,
        }) == 35)
        assert(mhr.magicAccuracyFromElementFromParams({
            magicalElement = el.DARK, elementalMaccMod = 0, elementalStaffMod = 0,
        }) == 0)
    end)
end)

describe('magicAccuracyFromStatusEffectsFromParams', function()
    it('altruism sets white only', function()
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasAltruism = true, spellGroup = spellGroup.WHITE, altruismPower = 42,
        }) == 42)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasAltruism = true, spellGroup = spellGroup.BLACK, altruismPower = 42,
        }) == 0)
    end)

    it('focalization adds black only', function()
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasFocalization = true, spellGroup = spellGroup.BLACK, focalizationPower = 33,
        }) == 33)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasFocalization = true, spellGroup = spellGroup.WHITE, focalizationPower = 33,
        }) == 0)
    end)

    it('klimaform / seals gate correctly', function()
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasKlimaform = true, magicalElement = el.FIRE, weatherMatchesElement = true,
        }) == 15)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasKlimaform = true, magicalElement = el.FIRE, weatherMatchesElement = false,
        }) == 0)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasDivineEmblem = true, actorJob = job.PLD, skillType = skill.DIVINE_MAGIC,
        }) == 256)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasDivineEmblem = true, actorJob = job.WHM, skillType = skill.DIVINE_MAGIC,
        }) == 0)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasElementalSeal = true, skillType = skill.ELEMENTAL_MAGIC, magicalElement = el.FIRE,
        }) == 256)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasElementalSeal = true, skillType = skill.DARK_MAGIC, magicalElement = el.DARK,
        }) == 0)
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasDarkSeal = true, skillType = skill.DARK_MAGIC,
        }) == 256)
    end)

    it('stacks focal + klima + elemental seal', function()
        assert(mhr.magicAccuracyFromStatusEffectsFromParams({
            hasFocalization = true, spellGroup = spellGroup.BLACK, focalizationPower = 20,
            hasKlimaform = true, magicalElement = el.ICE, weatherMatchesElement = true,
            hasElementalSeal = true, skillType = skill.ELEMENTAL_MAGIC,
        }) == 20 + 15 + 256)
    end)
end)

describe('magicAccuracyFromMeritsFromParams', function()
    it('BLM elemental only', function()
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BLM, skillType = skill.ELEMENTAL_MAGIC, elementalMerit = 25,
        }) == 25)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BLM, skillType = skill.DARK_MAGIC, elementalMerit = 25,
        }) == 0)
    end)

    it('RDM dual cat1+cat2', function()
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.RDM, magicalElement = el.FIRE,
            elementalMerit = 15, magicAccMerit = 20,
        }) == 35)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.RDM, magicalElement = el.LIGHT,
            elementalMerit = 15, magicAccMerit = 20,
        }) == 20)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.RDM, magicalElement = el.WATER,
            elementalMerit = 10, magicAccMerit = 5,
        }) == 15)
    end)

    it('BRD troubadour ladder', function()
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BRD, skillType = skill.SINGING, hasTroubadour = true, troubadourMerit = 25,
        }) == 0)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BRD, skillType = skill.SINGING, hasTroubadour = true, troubadourMerit = 50,
        }) == 64)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BRD, skillType = skill.SINGING, hasTroubadour = true, troubadourMerit = 100,
        }) == 192)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BRD, skillType = skill.SINGING, hasTroubadour = false, troubadourMerit = 100,
        }) == 0)
    end)

    it('NIN and BLU skill gates', function()
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.NIN, skillType = skill.NINJUTSU, ninMerit = 30,
        }) == 30)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.BLU, skillType = skill.BLUE_MAGIC, bluMerit = 18,
        }) == 18)
        assert(mhr.magicAccuracyFromMeritsFromParams({
            actorJob = job.WHM, elementalMerit = 99, magicAccMerit = 99,
        }) == 0)
    end)
end)

describe('magicAccuracyFromJobPointsFromParams', function()
    it('WHM and BLM flat JP', function()
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.WHM, whmJP = 7,
        }) == 7)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.BLM, blmJP = 11,
        }) == 11)
    end)

    it('RDM saboteur enfeeble *2 + rdmJP', function()
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.RDM, skillType = skill.ENFEEBLING_MAGIC,
            hasSaboteur = true, saboteurJP = 4, rdmJP = 5,
        }) == 13)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.RDM, skillType = skill.ENFEEBLING_MAGIC,
            hasSaboteur = false, saboteurJP = 4, rdmJP = 5,
        }) == 5)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.RDM, skillType = skill.ELEMENTAL_MAGIC,
            hasSaboteur = true, saboteurJP = 4, rdmJP = 5,
        }) == 5)
    end)

    it('BRD singing and NIN ninjutsu', function()
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.BRD, skillType = skill.SINGING, songAccJP = 9,
        }) == 9)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.NIN, skillType = skill.NINJUTSU, ninjutsuAccJP = 14,
        }) == 14)
    end)

    it('SCH inverted strategem pairing', function()
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.SCH, spellGroup = spellGroup.WHITE, hasParsimony = true, strategemJP = 8,
        }) == 8)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.SCH, spellGroup = spellGroup.BLACK, hasPenury = true, strategemJP = 8,
        }) == 8)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.SCH, spellGroup = spellGroup.WHITE, hasPenury = true, strategemJP = 8,
        }) == 0)
        assert(mhr.magicAccuracyFromJobPointsFromParams({
            actorJob = job.SCH, spellGroup = spellGroup.BLACK, hasParsimony = true, strategemJP = 8,
        }) == 0)
    end)
end)

describe('magicAccuracyFromMagicBurstFromParams', function()
    it('requires actorStat and skillchainCount', function()
        assert(mhr.magicAccuracyFromMagicBurstFromParams({ actorStat = 0, skillchainCount = 1 }) == 0)
        assert(mhr.magicAccuracyFromMagicBurstFromParams({ actorStat = 50, skillchainCount = 0 }) == 0)
        assert(mhr.magicAccuracyFromMagicBurstFromParams({ actorStat = 1, skillchainCount = 1 }) == 100)
        assert(mhr.magicAccuracyFromMagicBurstFromParams({ actorStat = 100, skillchainCount = 3 }) == 100)
    end)
end)

describe('magicAccuracyFromDayWeatherElementFromParams', function()
    it('element none and no-proc', function()
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.NONE, forceDW = true, roll1to100 = 1,
            associatedSingleWeather = weather.HOT_SPELL,
        }) == 0)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, dayElement = el.FIRE, weather = weather.HOT_SPELL,
            associatedSingleWeather = weather.HOT_SPELL, roll1to100 = 34,
        }) == 0)
    end)

    it('roll/forceDW/obi gates', function()
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, dayElement = el.FIRE, roll1to100 = 33,
        }) == 5)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, dayElement = el.FIRE, forceDW = true, roll1to100 = 100,
        }) == 5)
        -- elemental obi: bonus only, weak day not penalized
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, dayElement = el.WATER, forceElementObi = true, roll1to100 = 100,
            elementWeakness = el.WATER,
        }) == 0)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, dayElement = el.WATER, forceDW = true, roll1to100 = 100,
            elementWeakness = el.WATER,
        }) == -5)
    end)

    it('weather bonuses and penalties with iridescence', function()
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, weather = weather.HOT_SPELL, roll1to100 = 1,
            associatedSingleWeather = weather.HOT_SPELL,
        }) == 5)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, weather = weather.HEAT_WAVE, roll1to100 = 1,
            associatedDoubleWeather = weather.HEAT_WAVE,
        }) == 10)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.ICE, weather = weather.SNOW, iridescence = 1, roll1to100 = 1,
            associatedSingleWeather = weather.SNOW,
        }) == 10)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, weather = weather.RAIN, roll1to100 = 1,
            oppositeSingleWeather = weather.RAIN,
        }) == -5)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.FIRE, weather = weather.SQUALL, roll1to100 = 1,
            oppositeDoubleWeather = weather.SQUALL,
        }) == -10)
        assert(mhr.magicAccuracyFromDayWeatherElementFromParams({
            magicalElement = el.LIGHT, weather = weather.GLOOM, iridescence = 1, roll1to100 = 1,
            oppositeSingleWeather = weather.GLOOM,
        }) == -10)
    end)
end)

describe('magicAccuracyFromTandemStrikeFromParams', function()
    it('gates tandem and pet master power', function()
        assert(mhr.magicAccuracyFromTandemStrikeFromParams({
            tandemActive = false, isPetWithMasterPC = true, masterPower = 20, selfPower = 10,
        }) == 0)
        assert(mhr.magicAccuracyFromTandemStrikeFromParams({
            tandemActive = true, isPetWithMasterPC = true, masterPower = 20, selfPower = 10,
        }) == 20)
        assert(mhr.magicAccuracyFromTandemStrikeFromParams({
            tandemActive = true, isPetWithMasterPC = false, masterPower = 20, selfPower = 10,
        }) == 10)
    end)
end)

describe('magicAccuracyFromFoodMultiplierFromParams', function()
    it('percent bonus with optional cap', function()
        assert(math.abs(mhr.magicAccuracyFromFoodMultiplierFromParams({ foodMaccP = 10, foodMaccCap = 0 }) - 1.10) < 1e-12)
        assert(math.abs(mhr.magicAccuracyFromFoodMultiplierFromParams({ foodMaccP = 50, foodMaccCap = 20 }) - 1.20) < 1e-12)
        assert(math.abs(mhr.magicAccuracyFromFoodMultiplierFromParams({ foodMaccP = 10, foodMaccCap = 25 }) - 1.10) < 1e-12)
        assert(mhr.magicAccuracyFromFoodMultiplierFromParams({ foodMaccP = 0, foodMaccCap = 0 }) == 1)
        assert(mhr.magicAccuracyFromFoodMultiplierFromParams({ foodMaccP = -10, foodMaccCap = 20 }) == 1)
    end)
end)

describe('magicAccuracyFromSoulVoiceMultiplierFromParams', function()
    it('SV/Marcato on eligible singing enfeebles', function()
        assert(mhr.magicAccuracyFromSoulVoiceMultiplierFromParams({
            effectId = effect.SLEEP_I, skillType = skill.SINGING, hasSoulVoice = true,
        }) == 2)
        assert(mhr.magicAccuracyFromSoulVoiceMultiplierFromParams({
            effectId = effect.NONE, skillType = skill.SINGING, hasMarcato = true,
        }) == 1.5)
        assert(mhr.magicAccuracyFromSoulVoiceMultiplierFromParams({
            effectId = effect.CHARM_I, skillType = skill.SINGING, hasSoulVoice = true, hasMarcato = true,
        }) == 2)
        assert(mhr.magicAccuracyFromSoulVoiceMultiplierFromParams({
            effectId = effect.SLEEP_I, skillType = skill.ELEMENTAL_MAGIC, hasSoulVoice = true,
        }) == 1)
        assert(mhr.magicAccuracyFromSoulVoiceMultiplierFromParams({
            effectId = 13, skillType = skill.SINGING, hasSoulVoice = true, hasMarcato = true,
        }) == 1)
    end)
end)

describe('calculateActorMagicAccuracyFromParams', function()
    it('sums halves and floors product', function()
        assert(mhr.calculateActorMagicAccuracyFromParams({
            base = 100, skill = 50, element = 10, statDiff = 5,
            effects = 15, merits = 8, jobPoints = 4, burst = 100,
            dayWeather = 10, tandem = 3, food = 1, soulVoice = 1,
        }) == 305)
        assert(mhr.calculateActorMagicAccuracyFromParams({
            base = 100, skill = 50, food = 1.1, soulVoice = 2,
        }) == 330)
        assert(mhr.calculateActorMagicAccuracyFromParams({
            base = 101, food = 1.05, soulVoice = 1,
        }) == 106)
    end)
end)
