-----------------------------------
-- Global file for magic based skills magic hit rate.
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.magicHitRate = xi.combat.magicHitRate or {}

-----------------------------------
-- Pure formula helpers (OmegaXI slice 6678)
-- Dual-wired so Go internal/magichitrate and LSB tests share one surface.
-----------------------------------

-- Resistance rank table bounds (utils.clamp on calculated rank).
local RESIST_RANK_MIN = -3
local RESIST_RANK_MAX = 11

-- Auto-resist / floor-MHR rank thresholds from calculateResistRate.
local RANK_AUTO_RESIST = 11      -- status → 0, nuke → 0.25
local RANK_FLOOR_HIT_RATE = 10   -- skip MACC/MEVA assembly; MHR floor 0.05
local NUKE_AUTO_RESIST_FACTOR = 0.25
local MAGIC_HIT_RATE_FLOOR = 0.05
local MAGIC_HIT_RATE_CAP = 0.95
local STAT_DIFF_MACC_MIN = -30
local STAT_DIFF_MACC_MAX = 30
local MAX_RESIST_TIER_DEFAULT = 3

-- Resist-rank → MEVA multiplier table (research-cited; ranks 10/11 untestable live).
xi.combat.magicHitRate.resistRankMultiplierTable =
{
    [-3] = 0.95,
    [-2] = 0.96019,
    [-1] = 0.98,
    [ 0] = 1,
    [ 1] = 1.023,
    [ 2] = 1.049,
    [ 3] = 1.0905,
    [ 4] = 1.126,
    [ 5] = 1.2075,
    [ 6] = 1.3475,
    [ 7] = 1.70065,
    [ 8] = 2.141,
    [ 9] = 2.2,
    [10] = 2.275, -- Impossible to test since 'Magic Hit Rate' is floored to 5% at this point.
    [11] = 2.35,  -- Impossible to test since 'Magic Hit Rate' is floored to 5% at this point.
}

-- Clamp resistance rank to [-3, 11].
xi.combat.magicHitRate.clampResistRank = function(rank)
    return utils.clamp(rank, RESIST_RANK_MIN, RESIST_RANK_MAX)
end

-- MEVA multiplier for a resistance rank (clamped first).
xi.combat.magicHitRate.resistRankMultiplier = function(rank)
    rank = xi.combat.magicHitRate.clampResistRank(rank)
    local mult = xi.combat.magicHitRate.resistRankMultiplierTable[rank]

    if mult == nil then
        return 1
    end

    return mult
end

-- Pure stat-diff MACC ladder once actorStat - targetStat is known.
-- When actorStat was 0 the host returns 0 before calling this.
xi.combat.magicHitRate.magicAccuracyFromStatDifference = function(statDiff)
    local magicAcc = 0

    if statDiff <= -31 then
        magicAcc = -20 + (statDiff + 30) / 4
    elseif statDiff <= -11 then
        magicAcc = -10 + (statDiff + 10) / 2
    elseif statDiff < 11 then -- Between -11 and 11
        magicAcc = statDiff
    elseif statDiff >= 31 then
        magicAcc = 20 + (statDiff - 30) / 4
    elseif statDiff >= 11 then
        magicAcc = 10 + (statDiff - 10) / 2
    end

    return utils.clamp(magicAcc, STAT_DIFF_MACC_MIN, STAT_DIFF_MACC_MAX)
end

-- Pure magic hit rate once MACC and MEVA are known:
--   diff = macc - meva; if diff < 0 then floor(diff/2); clamp((50+diff)/100, 0.05, 0.95)
xi.combat.magicHitRate.calculateMagicHitRate = function(actorMagicAccuracy, targetMagicEvasion)
    local magicHitRate = actorMagicAccuracy - targetMagicEvasion

    if magicHitRate < 0 then
        magicHitRate = math.floor(magicHitRate / 2)
    end

    return utils.clamp((50 + magicHitRate) / 100, MAGIC_HIT_RATE_FLOOR, MAGIC_HIT_RATE_CAP)
end

-- PC elemental-MEVA screen gate for successive resist tiers (non-PC always 3).
xi.combat.magicHitRate.maxResistTier = function(isPC, elementalMeva)
    if not isPC then
        return MAX_RESIST_TIER_DEFAULT
    end

    if elementalMeva < 0 then
        return 1
    elseif elementalMeva == 0 then
        return 2
    end

    return MAX_RESIST_TIER_DEFAULT
end

-- Map completed resist-tier count → damage/effect factor 1/2^tier.
xi.combat.magicHitRate.resistanceFactorFromTier = function(resistTier)
    if resistTier <= 0 then
        return 1
    end

    return 1 / (2 ^ resistTier)
end

-- Pure multi-roll resist-tier count. rolls[i] true means that roll resisted
-- (math.random() > magicHitRate). Stops at first non-resist or after maxTiers.
xi.combat.magicHitRate.countResistTiers = function(maxTiers, rolls)
    if maxTiers < 0 then
        maxTiers = 0
    end

    local tier = 0
    rolls = rolls or {}

    for i = 1, maxTiers do
        if not rolls[i] then
            break
        end

        tier = tier + 1
    end

    return tier
end

-- Rank ≥ 11 auto-resist short-circuit. Returns factor, autoApplied.
-- Status (effectId > 0) → 0; nuke → 0.25. Rank < 11 → 0, false.
xi.combat.magicHitRate.autoResistFactor = function(rank, effectId)
    if rank < RANK_AUTO_RESIST then
        return 0, false
    end

    if effectId and effectId > 0 then
        return 0, true
    end

    return NUKE_AUTO_RESIST_FACTOR, true
end

-- Rank ∈ [10, 11): skip MACC/MEVA assembly and use floor magic hit rate.
xi.combat.magicHitRate.skipHitRateAssembly = function(rank)
    return rank >= RANK_FLOOR_HIT_RATE and rank < RANK_AUTO_RESIST
end

-----------------------------------
-- Calculate Target Resistance Rank
-----------------------------------
local function calculateTargetResistanceRank(actor, target, params)
    -- Early return: Players don't use resistance ranks.
    if target:isPC() then
        return 0
    end

    -- Calculate what modifier to use.
    local resistanceRankMod = 0 -- Modifier ID of the resistance rank to use.
    if params.effectId > 0 then -- Check if it's an effect.
        resistanceRankMod = xi.data.statusEffect.getAssociatedResistanceRankModifier(params.effectId, params.magicalElement)
    end

    if resistanceRankMod == 0 then -- If it's an effect and this is 0, try with element.
        resistanceRankMod = xi.data.element.getElementalResistanceRankModifier(params.magicalElement)
    end

    -- Fetch resistance rank.
    local resistanceRank = target:getMod(resistanceRankMod)

    -- Apply possible resistance rank modifications.
    if params.effectId > 0 then
        resistanceRank = resistanceRank - target:getMod(xi.data.statusEffect.getAssociatedImmunobreakModifier(params.effectId)) -- Apply immunobreak modification.
    end

    return xi.combat.magicHitRate.clampResistRank(resistanceRank)
end

-----------------------------------
-- Calculate Actor Magic Accuracy
-- Pure contribution helpers dual-wired to OmegaXI internal/magacc (slice 6705).
-----------------------------------

xi.combat.magicHitRate.klimaformBonus = 15
xi.combat.magicHitRate.sealBonus = 256
xi.combat.magicHitRate.dayWeatherProcChance = 33
xi.combat.magicHitRate.singleWeatherAcc = 5
xi.combat.magicHitRate.doubleWeatherAcc = 10
xi.combat.magicHitRate.dayElementAcc = 5
xi.combat.magicHitRate.iridescenceAccStep = 5
xi.combat.magicHitRate.magicBurstAccBonus = 100
xi.combat.magicHitRate.soulVoiceMultiplier = 2
xi.combat.magicHitRate.marcatoMultiplier = 1.5

-- Pure magicAccuracyFromSkill once skill levels / entity class are injected.
-- params: skillType, skillLevel, skillRank, mainLvl, isMob, isPC, rangeSkillType, rangeSkillLvl
xi.combat.magicHitRate.magicAccuracyFromSkillFromParams = function(params)
    local magicAcc = 0
    local skillType = params.skillType or 0

    if skillType > 0 then
        magicAcc = params.skillLevel or 0

        if magicAcc == 0 and params.isMob then
            magicAcc = xi.data.skillLevel.getSkillCap(params.mainLvl or 0, xi.skillRank.A_PLUS)
        end

        if skillType == xi.skill.SINGING then
            if params.isPC then
                local rangeType = params.rangeSkillType or 0
                if rangeType == xi.skill.WIND_INSTRUMENT then
                    magicAcc = magicAcc + (params.rangeSkillLvl or 0)
                elseif rangeType == xi.skill.STRING_INSTRUMENT then
                    magicAcc = magicAcc + math.floor((params.rangeSkillLvl or 0) / 2)
                end
            else
                magicAcc = magicAcc * 2
            end
        end
    elseif (params.skillRank or 0) > 0 then
        magicAcc = xi.data.skillLevel.getSkillCap(params.mainLvl or 0, params.skillRank)
    else
        magicAcc = xi.data.skillLevel.getSkillCap(params.mainLvl or 0, xi.skillRank.A_PLUS)
    end

    return magicAcc
end

-- Pure magicAccuracyFromElement once elemental MACC/staff mods are injected.
xi.combat.magicHitRate.magicAccuracyFromElementFromParams = function(params)
    if (params.magicalElement or 0) <= xi.element.NONE then
        return 0
    end

    return (params.elementalMaccMod or 0) + (params.elementalStaffMod or 0) * 10
end

-- Pure magicAccuracyFromStatusEffects inject form.
xi.combat.magicHitRate.magicAccuracyFromStatusEffectsFromParams = function(params)
    local magicAcc = 0

    if params.hasAltruism and params.spellGroup == xi.magic.spellGroup.WHITE then
        magicAcc = params.altruismPower or 0
    end

    if params.hasFocalization and params.spellGroup == xi.magic.spellGroup.BLACK then
        magicAcc = magicAcc + (params.focalizationPower or 0)
    end

    if
        params.hasKlimaform and
        (params.magicalElement or 0) > 0 and
        params.weatherMatchesElement
    then
        magicAcc = magicAcc + xi.combat.magicHitRate.klimaformBonus
    end

    if
        params.hasDivineEmblem and
        params.actorJob == xi.job.PLD and
        params.skillType == xi.skill.DIVINE_MAGIC
    then
        magicAcc = magicAcc + xi.combat.magicHitRate.sealBonus
    end

    if
        params.hasElementalSeal and
        params.skillType ~= xi.skill.DARK_MAGIC and
        params.skillType ~= xi.skill.DIVINE_MAGIC and
        (params.magicalElement or 0) > 0
    then
        magicAcc = magicAcc + xi.combat.magicHitRate.sealBonus
    end

    if params.hasDarkSeal and params.skillType == xi.skill.DARK_MAGIC then
        magicAcc = magicAcc + xi.combat.magicHitRate.sealBonus
    end

    return magicAcc
end

-- Pure magicAccuracyFromMerits inject form.
xi.combat.magicHitRate.magicAccuracyFromMeritsFromParams = function(params)
    local job = params.actorJob or 0
    local skillType = params.skillType or 0
    local element = params.magicalElement or 0

    if job == xi.job.BLM then
        if skillType == xi.skill.ELEMENTAL_MAGIC then
            return params.elementalMerit or 0
        end
    elseif job == xi.job.RDM then
        local magicAcc = 0
        if element >= xi.element.FIRE and element <= xi.element.WATER then
            magicAcc = params.elementalMerit or 0
        end
        return magicAcc + (params.magicAccMerit or 0)
    elseif job == xi.job.BRD then
        if skillType == xi.skill.SINGING and params.hasTroubadour then
            return 64 * ((params.troubadourMerit or 0) / 25 - 1)
        end
    elseif job == xi.job.NIN then
        if skillType == xi.skill.NINJUTSU then
            return params.ninMerit or 0
        end
    elseif job == xi.job.BLU then
        if skillType == xi.skill.BLUE_MAGIC then
            return params.bluMerit or 0
        end
    end

    return 0
end

-- Pure magicAccuracyFromJobPoints inject form.
xi.combat.magicHitRate.magicAccuracyFromJobPointsFromParams = function(params)
    local job = params.actorJob or 0
    local skillType = params.skillType or 0
    local spellGroup = params.spellGroup or 0

    if job == xi.job.WHM then
        return params.whmJP or 0
    elseif job == xi.job.BLM then
        return params.blmJP or 0
    elseif job == xi.job.RDM then
        local magicAcc = 0
        if skillType == xi.skill.ENFEEBLING_MAGIC and params.hasSaboteur then
            magicAcc = (params.saboteurJP or 0) * 2
        end
        return magicAcc + (params.rdmJP or 0)
    elseif job == xi.job.BRD then
        if skillType == xi.skill.SINGING then
            return params.songAccJP or 0
        end
    elseif job == xi.job.NIN then
        if skillType == xi.skill.NINJUTSU then
            return params.ninjutsuAccJP or 0
        end
    elseif job == xi.job.SCH then
        if
            (spellGroup == xi.magic.spellGroup.WHITE and params.hasParsimony) or
            (spellGroup == xi.magic.spellGroup.BLACK and params.hasPenury)
        then
            return params.strategemJP or 0
        end
    end

    return 0
end

-- Pure magicAccuracyFromMagicBurst: actorStat + skillchainCount inject.
xi.combat.magicHitRate.magicAccuracyFromMagicBurstFromParams = function(params)
    if (params.actorStat or 0) == 0 or (params.skillchainCount or 0) <= 0 then
        return 0
    end

    return xi.combat.magicHitRate.magicBurstAccBonus
end

-- Pure day/weather MACC once element/day/weather/iridescence/force/roll are injected.
xi.combat.magicHitRate.magicAccuracyFromDayWeatherElementFromParams = function(params)
    local element = params.magicalElement or 0
    if element <= xi.element.NONE then
        return 0
    end

    local applyBonuses = false
    local applyPenalties = false
    local roll = params.roll1to100 or 0

    if roll <= xi.combat.magicHitRate.dayWeatherProcChance or params.forceDW then
        applyBonuses = true
        applyPenalties = true
    elseif params.forceElementObi then
        applyBonuses = true
    end

    local magicAcc = 0
    local weather = params.weather or 0
    local dayElement = params.dayElement or 0
    local iri = params.iridescence or 0
    local singleW = params.associatedSingleWeather or 0
    local doubleW = params.associatedDoubleWeather or 0
    local oppSingle = params.oppositeSingleWeather or 0
    local oppDouble = params.oppositeDoubleWeather or 0
    local weakness = params.elementWeakness or 0

    if applyBonuses then
        -- Associated weather injects are non-zero for valid elements (host always
        -- supplies them). Guard > 0 so unset injects do not match weather==0.
        if singleW > 0 and weather == singleW then
            magicAcc = magicAcc + xi.combat.magicHitRate.singleWeatherAcc + iri * xi.combat.magicHitRate.iridescenceAccStep
        elseif doubleW > 0 and weather == doubleW then
            magicAcc = magicAcc + xi.combat.magicHitRate.doubleWeatherAcc + iri * xi.combat.magicHitRate.iridescenceAccStep
        end
        if dayElement == element then
            magicAcc = magicAcc + xi.combat.magicHitRate.dayElementAcc
        end
    end

    if applyPenalties then
        if oppSingle > 0 and weather == oppSingle then
            magicAcc = magicAcc - xi.combat.magicHitRate.singleWeatherAcc - iri * xi.combat.magicHitRate.iridescenceAccStep
        elseif oppDouble > 0 and weather == oppDouble then
            magicAcc = magicAcc - xi.combat.magicHitRate.doubleWeatherAcc - iri * xi.combat.magicHitRate.iridescenceAccStep
        end
        if weakness > 0 and dayElement == weakness then
            magicAcc = magicAcc - xi.combat.magicHitRate.dayElementAcc
        end
    end

    return magicAcc
end

-- Pure tandem strike inject.
xi.combat.magicHitRate.magicAccuracyFromTandemStrikeFromParams = function(params)
    if not params.tandemActive then
        return 0
    end
    if params.isPetWithMasterPC then
        return params.masterPower or 0
    end
    return params.selfPower or 0
end

-- Pure food multiplier inject (raw FOOD_MACCP / FOOD_MACC_CAP).
xi.combat.magicHitRate.magicAccuracyFromFoodMultiplierFromParams = function(params)
    local foodMagicAccBonus = (params.foodMaccP or 0) / 100
    local foodMagicAccCap = (params.foodMaccCap or 0) / 100

    if foodMagicAccCap > 0 then
        foodMagicAccBonus = utils.clamp(foodMagicAccBonus, 0, foodMagicAccCap)
    end

    return 1 + foodMagicAccBonus
end

-- Pure Soul Voice / Marcato multiplier inject.
xi.combat.magicHitRate.magicAccuracyFromSoulVoiceMultiplierFromParams = function(params)
    local effectId = params.effectId
    local skillType = params.skillType or 0

    if skillType ~= xi.skill.SINGING then
        return 1
    end

    if
        effectId == xi.effect.SLEEP_I or
        effectId == xi.effect.NONE or
        effectId == xi.effect.CHARM_I
    then
        if params.hasSoulVoice then
            return xi.combat.magicHitRate.soulVoiceMultiplier
        elseif params.hasMarcato then
            return xi.combat.magicHitRate.marcatoMultiplier
        end
    end

    return 1
end

-- Pure actor MACC product once all contribution halves are injected.
xi.combat.magicHitRate.calculateActorMagicAccuracyFromParams = function(params)
    local sum =
        (params.base or 0) +
        (params.skill or 0) +
        (params.element or 0) +
        (params.statDiff or 0) +
        (params.effects or 0) +
        (params.merits or 0) +
        (params.jobPoints or 0) +
        (params.burst or 0) +
        (params.dayWeather or 0) +
        (params.tandem or 0)

    return math.floor(sum * (params.food or 1) * (params.soulVoice or 1))
end

-----------------------------------
-- Entity hosts for actor MACC
-----------------------------------

local function magicAccuracyFromSkill(actor, params)
    local skillLevel = 0
    local rangeSkillType = 0
    local rangeSkillLvl = 0

    if (params.skillType or 0) > 0 then
        skillLevel = actor:getSkillLevel(params.skillType)
        if params.skillType == xi.skill.SINGING and actor:isPC() then
            rangeSkillType = actor:getWeaponSkillType(xi.slot.RANGED)
            if
                rangeSkillType == xi.skill.WIND_INSTRUMENT or
                rangeSkillType == xi.skill.STRING_INSTRUMENT
            then
                rangeSkillLvl = actor:getSkillLevel(rangeSkillType)
            end
        end
    end

    return xi.combat.magicHitRate.magicAccuracyFromSkillFromParams({
        skillType      = params.skillType,
        skillLevel     = skillLevel,
        skillRank      = params.skillRank,
        mainLvl        = actor:getMainLvl(),
        isMob          = actor:isMob(),
        isPC           = actor:isPC(),
        rangeSkillType = rangeSkillType,
        rangeSkillLvl  = rangeSkillLvl,
    })
end

local function magicAccuracyFromElement(actor, params)
    local elementalMacc = 0
    local elementalStaff = 0
    if (params.magicalElement or 0) > xi.element.NONE then
        elementalMacc = actor:getMod(xi.data.element.getElementalMACCModifier(params.magicalElement))
        elementalStaff = actor:getMod(xi.data.element.getElementalStaffModifier(params.magicalElement))
    end

    return xi.combat.magicHitRate.magicAccuracyFromElementFromParams({
        magicalElement     = params.magicalElement,
        elementalMaccMod   = elementalMacc,
        elementalStaffMod  = elementalStaff,
    })
end

local function magicAccuracyFromStatDifference(actor, target, params)
    if params.actorStat == 0 then
        return 0
    end

    local statDiff = actor:getStat(params.actorStat) - target:getStat(params.targetStat)
    return xi.combat.magicHitRate.magicAccuracyFromStatDifference(statDiff)
end

local function magicAccuracyFromStatusEffects(actor, params)
    local weather = actor:getWeather()
    local weatherMatch = false
    if (params.magicalElement or 0) > 0 then
        weatherMatch =
            weather == xi.data.element.getAssociatedSingleWeather(params.magicalElement) or
            weather == xi.data.element.getAssociatedDoubleWeather(params.magicalElement)
    end

    local altruismPower = 0
    if actor:hasStatusEffect(xi.effect.ALTRUISM) then
        altruismPower = actor:getStatusEffect(xi.effect.ALTRUISM):getPower()
    end
    local focalizationPower = 0
    if actor:hasStatusEffect(xi.effect.FOCALIZATION) then
        focalizationPower = actor:getStatusEffect(xi.effect.FOCALIZATION):getPower()
    end

    return xi.combat.magicHitRate.magicAccuracyFromStatusEffectsFromParams({
        spellGroup             = params.spellGroup,
        magicalElement         = params.magicalElement,
        skillType              = params.skillType,
        actorJob               = actor:getMainJob(),
        hasAltruism            = actor:hasStatusEffect(xi.effect.ALTRUISM),
        altruismPower          = altruismPower,
        hasFocalization        = actor:hasStatusEffect(xi.effect.FOCALIZATION),
        focalizationPower      = focalizationPower,
        hasKlimaform           = actor:hasStatusEffect(xi.effect.KLIMAFORM),
        weatherMatchesElement  = weatherMatch,
        hasDivineEmblem        = actor:hasStatusEffect(xi.effect.DIVINE_EMBLEM),
        hasElementalSeal       = actor:hasStatusEffect(xi.effect.ELEMENTAL_SEAL),
        hasDarkSeal            = actor:hasStatusEffect(xi.effect.DARK_SEAL),
    })
end

local function magicAccuracyFromMerits(actor, params)
    local job = actor:getMainJob()
    local elementalMerit = 0
    if job == xi.job.BLM then
        elementalMerit = actor:getMerit(xi.merit.ELEMENTAL_MAGIC_ACCURACY)
    elseif job == xi.job.RDM and
        (params.magicalElement or 0) >= xi.element.FIRE and
        (params.magicalElement or 0) <= xi.element.WATER
    then
        elementalMerit = actor:getMerit(xi.data.element.getElementalAccuracyMerit(params.magicalElement))
    end

    return xi.combat.magicHitRate.magicAccuracyFromMeritsFromParams({
        actorJob         = job,
        skillType        = params.skillType,
        magicalElement   = params.magicalElement,
        elementalMerit   = elementalMerit,
        magicAccMerit    = actor:getMerit(xi.merit.MAGIC_ACCURACY),
        troubadourMerit  = actor:getMerit(xi.merit.TROUBADOUR),
        hasTroubadour    = actor:hasStatusEffect(xi.effect.TROUBADOUR),
        ninMerit         = actor:getMerit(xi.merit.NIN_MAGIC_ACCURACY),
        bluMerit         = actor:getMerit(xi.merit.MAGICAL_ACCURACY),
    })
end

local function magicAccuracyFromJobPoints(actor, params)
    return xi.combat.magicHitRate.magicAccuracyFromJobPointsFromParams({
        actorJob       = actor:getMainJob(),
        skillType      = params.skillType,
        spellGroup     = params.spellGroup,
        hasSaboteur    = actor:hasStatusEffect(xi.effect.SABOTEUR),
        hasParsimony   = actor:hasStatusEffect(xi.effect.PARSIMONY),
        hasPenury      = actor:hasStatusEffect(xi.effect.PENURY),
        whmJP          = actor:getJobPointLevel(xi.jp.WHM_MAGIC_ACC_BONUS),
        blmJP          = actor:getJobPointLevel(xi.jp.BLM_MAGIC_ACC_BONUS),
        rdmJP          = actor:getJobPointLevel(xi.jp.RDM_MAGIC_ACC_BONUS),
        saboteurJP     = actor:getJobPointLevel(xi.jp.SABOTEUR_EFFECT),
        songAccJP      = actor:getJobPointLevel(xi.jp.SONG_ACC_BONUS),
        ninjutsuAccJP  = actor:getJobPointLevel(xi.jp.NINJITSU_ACC_BONUS),
        strategemJP    = actor:getJobPointLevel(xi.jp.STRATEGEM_EFFECT_I),
    })
end

local function magicAccuracyFromMagicBurst(target, params)
    local _, skillchainCount = xi.magicburst.formMagicBurst(target, params.magicalElement)
    return xi.combat.magicHitRate.magicAccuracyFromMagicBurstFromParams({
        actorStat        = params.actorStat,
        skillchainCount  = skillchainCount,
    })
end

local function magicAccuracyFromDayWeatherElement(actor, params)
    local element = params.magicalElement or 0
    local forceDW = actor:getMod(xi.mod.FORCE_DW_BONUS_PENALTY) >= 1
    local forceObi = false
    if element > xi.element.NONE then
        forceObi = actor:getMod(xi.data.element.getForcedDayOrWeatherBonusModifier(element)) >= 1
    end

    return xi.combat.magicHitRate.magicAccuracyFromDayWeatherElementFromParams({
        magicalElement           = element,
        dayElement               = VanadielDayElement(),
        weather                  = actor:getWeather(),
        iridescence              = actor:getMod(xi.mod.IRIDESCENCE),
        forceDW                  = forceDW,
        forceElementObi          = forceObi,
        roll1to100               = math.random(1, 100),
        associatedSingleWeather  = element > 0 and xi.data.element.getAssociatedSingleWeather(element) or 0,
        associatedDoubleWeather  = element > 0 and xi.data.element.getAssociatedDoubleWeather(element) or 0,
        oppositeSingleWeather    = element > 0 and xi.data.element.getOppositeSingleWeather(element) or 0,
        oppositeDoubleWeather    = element > 0 and xi.data.element.getOppositeDoubleWeather(element) or 0,
        elementWeakness          = element > 0 and xi.data.element.getElementWeakness(element) or 0,
    })
end

local function magicAccuracyFromTandemStrike(actor)
    local tandemActive = actor:isTandemActive()
    local isPetWithMasterPC = false
    local masterPower = 0
    local selfPower = actor:getMod(xi.mod.TANDEM_STRIKE_POWER)

    if tandemActive then
        local master = actor:getMaster()
        if master and master:isPC() then
            isPetWithMasterPC = true
            masterPower = master:getMod(xi.mod.TANDEM_STRIKE_POWER)
        end
    end

    return xi.combat.magicHitRate.magicAccuracyFromTandemStrikeFromParams({
        tandemActive       = tandemActive,
        isPetWithMasterPC  = isPetWithMasterPC,
        masterPower        = masterPower,
        selfPower          = selfPower,
    })
end

local function magicAccuracyFromFoodMultiplier(actor)
    return xi.combat.magicHitRate.magicAccuracyFromFoodMultiplierFromParams({
        foodMaccP   = actor:getMod(xi.mod.FOOD_MACCP),
        foodMaccCap = actor:getMod(xi.mod.FOOD_MACC_CAP),
    })
end

local function magicAccuracyFromSoulVoiceMultiplier(actor, params)
    return xi.combat.magicHitRate.magicAccuracyFromSoulVoiceMultiplierFromParams({
        effectId     = params.effectId,
        skillType    = params.skillType,
        hasSoulVoice = actor:hasStatusEffect(xi.effect.SOUL_VOICE),
        hasMarcato   = actor:hasStatusEffect(xi.effect.MARCATO),
    })
end

local function calculateActorMagicAccuracy(actor, target, params)
    return xi.combat.magicHitRate.calculateActorMagicAccuracyFromParams({
        base       = actor:getMod(xi.mod.MACC) + actor:getILvlMacc(xi.slot.MAIN) + (params.bonusMacc or 0),
        skill      = magicAccuracyFromSkill(actor, params),
        element    = magicAccuracyFromElement(actor, params),
        statDiff   = magicAccuracyFromStatDifference(actor, target, params),
        effects    = magicAccuracyFromStatusEffects(actor, params),
        merits     = magicAccuracyFromMerits(actor, params),
        jobPoints  = magicAccuracyFromJobPoints(actor, params),
        burst      = magicAccuracyFromMagicBurst(target, params),
        dayWeather = magicAccuracyFromDayWeatherElement(actor, params),
        tandem     = magicAccuracyFromTandemStrike(actor),
        food       = magicAccuracyFromFoodMultiplier(actor),
        soulVoice  = magicAccuracyFromSoulVoiceMultiplier(actor, params),
    })
end

-----------------------------------
-- Calculate Target Magic Evasion
-----------------------------------
local function calculateTargetMagicEvasion(actor, target, params)
    local magicEva = target:getMod(xi.mod.MEVA) -- Base MACC.

    -- Elemental magic evasion. All actions and effects have an associated element.
    if params.magicalElement ~= xi.element.NONE then
        magicEva = magicEva + target:getMod(xi.data.element.getElementalMEVAModifier(params.magicalElement))
    end

    -- Magic evasion against specific status effects.
    if params.effectId > 0 then
        magicEva = magicEva + target:getMod(xi.data.statusEffect.getAssociatedMagicEvasionModifier(params.effectId)) + target:getMod(xi.mod.STATUS_MEVA)
    end

    -- Level correction. Target gets a bonus the higher the level if it's a mob. Never a penalty.
    if
        not target:isPC() and
        xi.data.levelCorrection.isLevelCorrectedZone(actor)
    then
        magicEva = magicEva + utils.clamp(target:getMainLvl() - actor:getMainLvl(), 0, 100) * 4
    end

    -- Apply resistance rank multiplier.
    magicEva = math.floor(magicEva * xi.combat.magicHitRate.resistRankMultiplier(params.resistanceRank))

    return magicEva
end

-----------------------------------
-- Magic Hit Rate. The function gets fed the result of both functions above.
-----------------------------------
local function calculateMagicHitRate(params)
    return xi.combat.magicHitRate.calculateMagicHitRate(params.actorMagicAccuracy, params.targetMagicEvasion)
end

-----------------------------------
-- Calculate resist rate.
-----------------------------------
local function calculateResistanceFactor(actor, target, params)
    -- Early return: Magic shield.
    if target:hasStatusEffect(xi.effect.MAGIC_SHIELD, 0) then
        return 0
    end

    -- Early return: Cannot resist non-elemental magic.
    if params.magicalElement == xi.element.NONE then
        return 1
    end

    -- Calculate max allowed resist tier (PC elemental MEVA screen gate).
    local elementalMeva = 0
    if target:isPC() then
        elementalMeva = target:getMod(xi.data.element.getElementalMEVAModifier(params.magicalElement)) or 0
    end

    local maxResistTier = xi.combat.magicHitRate.maxResistTier(target:isPC(), elementalMeva)

    -- Calculate first N resist tiers.
    -- Notes: https://wiki-ffo-jp.translate.goog/html/795.html?_x_tr_sl=ja&_x_tr_tl=en&_x_tr_hl=en&_x_tr_pto=sc
    local rolls = {}
    for i = 1, maxResistTier do
        -- true = resisted (math.random() > magicHitRate)
        if math.random() > params.magicHitRate then
            rolls[i] = true
        else
            break
        end
    end

    local resistTier = xi.combat.magicHitRate.countResistTiers(maxResistTier, rolls)

    return xi.combat.magicHitRate.resistanceFactorFromTier(resistTier)
end

-----------------------------------
-- Resist rate helper function.
-----------------------------------
local function validateParameters(actor, target, fedData)
    local params = {}

    -- Action information.
    params.effectId           = fedData.effectId or 0
    params.magicalElement     = fedData.magicalElement or xi.element.NONE
    params.bonusMacc          = fedData.bonusMacc or 0
    params.actorStat          = fedData.actorStat or 0
    params.targetStat         = fedData.targetStat or params.actorStat
    params.skillType          = fedData.skillType or 0
    params.skillRank          = fedData.skillRank or 0
    params.spellGroup         = fedData.spellGroup or 0

    -- Initialize future parameters.
    params.resistanceRank     = 0
    params.actorMagicAccuracy = 0
    params.targetMagicEvasion = 0
    params.magicHitRate       = 0.05

    return params
end

xi.combat.magicHitRate.calculateResistRate = function(actor, target, spellGroup, skillType, skillRank, actionElement, statUsed, effectId, bonusMacc)
    local fedData = -- Temporal measure: Table fed parameters. TODO: Feed a table to this function directly.
    {
        effectId       = utils.defaultIfNil(effectId, 0),
        magicalElement = utils.defaultIfNil(actionElement, 0),
        bonusMacc      = utils.defaultIfNil(bonusMacc, 0),
        actorStat      = utils.defaultIfNil(statUsed, 0),
        skillType      = utils.defaultIfNil(skillType, 0),
        skillRank      = utils.defaultIfNil(skillRank, 0),
        spellGroup     = utils.defaultIfNil(spellGroup, 0),
    }

    -- Validate fed parameters.
    local params = validateParameters(actor, target, fedData)

    -- Calculate and table resistance rank.
    params.resistanceRank = calculateTargetResistanceRank(actor, target, params)

    -- Early return: Auto-resist (rank ≥ 11).
    local autoFactor, isAuto = xi.combat.magicHitRate.autoResistFactor(params.resistanceRank, params.effectId)
    if isAuto then
        return autoFactor
    end

    -- Early return: MHR is floored to 0.05. Skip calculating it (rank ∈ [10, 11)).
    if xi.combat.magicHitRate.skipHitRateAssembly(params.resistanceRank) then
        params.magicHitRate = MAGIC_HIT_RATE_FLOOR
        return calculateResistanceFactor(actor, target, params)
    end

    -- Calculate and table MACC, MEVA and MHR.
    params.actorMagicAccuracy = calculateActorMagicAccuracy(actor, target, params)
    params.targetMagicEvasion = calculateTargetMagicEvasion(actor, target, params)
    params.magicHitRate       = calculateMagicHitRate(params)

    return calculateResistanceFactor(actor, target, params)
end
