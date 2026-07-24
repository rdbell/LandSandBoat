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
-----------------------------------

-- Magic Accuracy from spell's skill.
local function magicAccuracyFromSkill(actor, params)
    local magicAcc = 0

    -- For known skills.
    if params.skillType > 0 then
        magicAcc = actor:getSkillLevel(params.skillType)

        -- If a mob is casting something its main/sub jobs cannot (i.e. JoL casting black magic) this is _exceptional_
        -- So give them A+ rank base magic accuracy
        if magicAcc == 0 and actor:isMob() then
            magicAcc = xi.data.skillLevel.getSkillCap(actor:getMainLvl(), xi.skillRank.A_PLUS)
        end

        if params.skillType == xi.skill.SINGING then
            if actor:isPC() then
                -- Add ranged skill level ONLY if it's an instrument.
                local rangeType = actor:getWeaponSkillType(xi.slot.RANGED)

                -- String instruments have half the skill effectiveness and amplify the AoE in exchange.
                if rangeType == xi.skill.WIND_INSTRUMENT then
                    magicAcc = magicAcc + actor:getSkillLevel(rangeType)
                elseif rangeType == xi.skill.STRING_INSTRUMENT then
                    magicAcc = magicAcc + math.floor(actor:getSkillLevel(rangeType) / 2)
                end

            else
                magicAcc = magicAcc * 2
            end
        end

    -- Made for bolts. Will probably see other uses.
    elseif params.skillRank > 0 then
        magicAcc = xi.data.skillLevel.getSkillCap(actor:getMainLvl(), params.skillRank)

    -- For mob skills / additional effects which don't have a skill.
    else
        magicAcc = xi.data.skillLevel.getSkillCap(actor:getMainLvl(), xi.skillRank.A_PLUS)
    end

    return magicAcc
end

-- Magic Accuracy from spell's element.
local function magicAccuracyFromElement(actor, params)
    -- Early return: No element.
    if params.magicalElement <= xi.element.NONE then
        return 0
    end

    return actor:getMod(xi.data.element.getElementalMACCModifier(params.magicalElement)) + actor:getMod(xi.data.element.getElementalStaffModifier(params.magicalElement)) * 10
end

-- Magic Accuracy from Stat Difference between caster and target.
local function magicAccuracyFromStatDifference(actor, target, params)
    if params.actorStat == 0 then
        return 0
    end

    local statDiff = actor:getStat(params.actorStat) - target:getStat(params.targetStat)

    return xi.combat.magicHitRate.magicAccuracyFromStatDifference(statDiff)
end

-- Magic Accuracy from Status Effects.
local function magicAccuracyFromStatusEffects(actor, params)
    local magicAcc     = 0
    local actorJob     = actor:getMainJob()
    local actorWeather = actor:getWeather()

    -- Altruism
    if
        actor:hasStatusEffect(xi.effect.ALTRUISM) and
        params.spellGroup == xi.magic.spellGroup.WHITE
    then
        magicAcc = actor:getStatusEffect(xi.effect.ALTRUISM):getPower()
    end

    -- Focalization
    if
        actor:hasStatusEffect(xi.effect.FOCALIZATION) and
        params.spellGroup == xi.magic.spellGroup.BLACK
    then
        magicAcc = magicAcc + actor:getStatusEffect(xi.effect.FOCALIZATION):getPower()
    end

    --Klimaform
    if
        actor:hasStatusEffect(xi.effect.KLIMAFORM) and
        params.magicalElement > 0 and
        (actorWeather == xi.data.element.getAssociatedSingleWeather(params.magicalElement) or actorWeather == xi.data.element.getAssociatedDoubleWeather(params.magicalElement))
    then
        magicAcc = magicAcc + 15
    end

    -- Apply Divine Emblem to Banish and Holy families
    if
        actor:hasStatusEffect(xi.effect.DIVINE_EMBLEM) and
        actorJob == xi.job.PLD and
        params.skillType == xi.skill.DIVINE_MAGIC
    then
        magicAcc = magicAcc + 256
    end

    -- Elemental seal
    if
        actor:hasStatusEffect(xi.effect.ELEMENTAL_SEAL) and
        params.skillType ~= xi.skill.DARK_MAGIC and
        params.skillType ~= xi.skill.DIVINE_MAGIC and
        params.magicalElement > 0
    then
        magicAcc = magicAcc + 256
    end

    -- Dark Seal
    if
        actor:hasStatusEffect(xi.effect.DARK_SEAL) and
        params.skillType == xi.skill.DARK_MAGIC
    then
        magicAcc = magicAcc + 256
    end

    return magicAcc
end

-- Magic Accuracy from Merits.
local function magicAccuracyFromMerits(actor, params)
    local magicAcc = 0
    local actorJob = actor:getMainJob()

    switch (actorJob) : caseof
    {
        [xi.job.BLM] = function()
            if params.skillType == xi.skill.ELEMENTAL_MAGIC then
                magicAcc = actor:getMerit(xi.merit.ELEMENTAL_MAGIC_ACCURACY)
            end
        end,

        [xi.job.RDM] = function()
            -- Category 1
            if
                params.magicalElement >= xi.element.FIRE and
                params.magicalElement <= xi.element.WATER
            then
                magicAcc = actor:getMerit(xi.data.element.getElementalAccuracyMerit(params.magicalElement))
            end

            -- Category 2
            magicAcc = magicAcc + actor:getMerit(xi.merit.MAGIC_ACCURACY)
        end,

        [xi.job.BRD] = function()
            if
                params.skillType == xi.skill.SINGING and
                actor:hasStatusEffect(xi.effect.TROUBADOUR)
            then
                magicAcc = 64 * (actor:getMerit(xi.merit.TROUBADOUR) / 25 - 1)
            end
        end,

        [xi.job.NIN] = function()
            if params.skillType == xi.skill.NINJUTSU then
                magicAcc = actor:getMerit(xi.merit.NIN_MAGIC_ACCURACY)
            end
        end,

        [xi.job.BLU] = function()
            if params.skillType == xi.skill.BLUE_MAGIC then
                magicAcc = actor:getMerit(xi.merit.MAGICAL_ACCURACY)
            end
        end,
    }

    return magicAcc
end

-- Magic Accuracy from Job Points.
local function magicAccuracyFromJobPoints(actor, params)
    local magicAcc = 0
    local actorJob = actor:getMainJob()

    switch (actorJob) : caseof
    {
        [xi.job.WHM] = function()
            magicAcc = actor:getJobPointLevel(xi.jp.WHM_MAGIC_ACC_BONUS)
        end,

        [xi.job.BLM] = function()
            magicAcc = actor:getJobPointLevel(xi.jp.BLM_MAGIC_ACC_BONUS)
        end,

        [xi.job.RDM] = function()
            -- RDM Job Point: During saboteur, Enfeebling MACC +2
            if
                params.skillType == xi.skill.ENFEEBLING_MAGIC and
                actor:hasStatusEffect(xi.effect.SABOTEUR)
            then
                magicAcc = actor:getJobPointLevel(xi.jp.SABOTEUR_EFFECT) * 2
            end

            -- RDM Job Point: Magic Accuracy Bonus, All MACC + 1
            magicAcc = magicAcc + actor:getJobPointLevel(xi.jp.RDM_MAGIC_ACC_BONUS)
        end,

        [xi.job.BRD] = function()
            if params.skillType == xi.skill.SINGING then
                magicAcc = actor:getJobPointLevel(xi.jp.SONG_ACC_BONUS)
            end
        end,

        [xi.job.NIN] = function()
            if params.skillType == xi.skill.NINJUTSU then
                magicAcc = actor:getJobPointLevel(xi.jp.NINJITSU_ACC_BONUS)
            end
        end,

        [xi.job.SCH] = function()
            if
                (params.spellGroup == xi.magic.spellGroup.WHITE and actor:hasStatusEffect(xi.effect.PARSIMONY)) or
                (params.spellGroup == xi.magic.spellGroup.BLACK and actor:hasStatusEffect(xi.effect.PENURY))
            then
                magicAcc = actor:getJobPointLevel(xi.jp.STRATEGEM_EFFECT_I)
            end
        end,
    }

    return magicAcc
end

-- Magic Accuracy from Magic Burst.
local function magicAccuracyFromMagicBurst(target, params)
    if params.actorStat == 0 then
        return 0
    end

    local _, skillchainCount = xi.magicburst.formMagicBurst(target, params.magicalElement)
    if skillchainCount <= 0 then
        return 0
    end

    return 100
end

-- Magic Accuracy from Day and Weather Element.
local function magicAccuracyFromDayWeatherElement(actor, params)
    local magicAcc = 0

    -- Early return: Invalid element.
    if params.magicalElement <= xi.element.NONE then
        return magicAcc
    end

    -- Define what to apply.
    local applyBonuses   = false
    local applyPenalties = false

    if
        math.random(1, 100) <= 33 or                     -- Random. Applies to both bonuses and penalties.
        actor:getMod(xi.mod.FORCE_DW_BONUS_PENALTY) >= 1 -- Hachirin-no-Obi forces both bonuses and penalties.
    then
        applyBonuses   = true
        applyPenalties = true
    elseif actor:getMod(xi.data.element.getForcedDayOrWeatherBonusModifier(params.magicalElement)) >= 1 then -- Elemental Obis only force bonuses, not penalties.
        applyBonuses = true
    end

    -- Calculate bonuses/penalties.
    local dayElement   = VanadielDayElement()
    local actorWeather = actor:getWeather()

    if applyBonuses then
        if actorWeather == xi.data.element.getAssociatedSingleWeather(params.magicalElement) then
            magicAcc = magicAcc + 5 + actor:getMod(xi.mod.IRIDESCENCE) * 5
        elseif actorWeather == xi.data.element.getAssociatedDoubleWeather(params.magicalElement) then
            magicAcc = magicAcc + 10 + actor:getMod(xi.mod.IRIDESCENCE) * 5
        end

        if dayElement == params.magicalElement then
            magicAcc = magicAcc + 5
        end
    end

    if applyPenalties then
        if actorWeather == xi.data.element.getOppositeSingleWeather(params.magicalElement) then
            magicAcc = magicAcc - 5 - actor:getMod(xi.mod.IRIDESCENCE) * 5
        elseif actorWeather == xi.data.element.getOppositeDoubleWeather(params.magicalElement) then
            magicAcc = magicAcc - 10 - actor:getMod(xi.mod.IRIDESCENCE) * 5
        end

        if dayElement == xi.data.element.getElementWeakness(params.magicalElement) then
            magicAcc = magicAcc - 5
        end
    end

    return magicAcc
end

-- Magic Accuracy from Tandem Strike (BST trait).
local function magicAccuracyFromTandemStrike(actor)
    -- Early return: Can't apply Tandem Strike.
    if not actor:isTandemActive() then
        return 0
    end

    -- Actor is a pet, with a master. Fetch master modifier.
    local master = actor:getMaster()
    if master and master:isPC() then
        return master:getMod(xi.mod.TANDEM_STRIKE_POWER)
    end

    -- Actor is the master.
    return actor:getMod(xi.mod.TANDEM_STRIKE_POWER)
end

-- Magic Accuracy from Food.
local function magicAccuracyFromFoodMultiplier(actor)
    local foodMagicAccBonus = actor:getMod(xi.mod.FOOD_MACCP) / 100
    local foodMagicAccCap   = actor:getMod(xi.mod.FOOD_MACC_CAP) / 100

    if foodMagicAccCap > 0 then
        foodMagicAccBonus = utils.clamp(foodMagicAccBonus, 0, foodMagicAccCap)
    end

    return 1 + foodMagicAccBonus
end

local function magicAccuracyFromSoulVoiceMultiplier(actor, params)
    local effectTable =
    set{
        xi.effect.SLEEP_I, -- Lullabies
        xi.effect.NONE,    -- Magic Finale
        xi.effect.CHARM_I  -- Maiden's Virellai
    }

    if
        effectTable[params.effectId] and
        params.skillType == xi.skill.SINGING
    then
        if actor:hasStatusEffect(xi.effect.SOUL_VOICE) then
            return 2
        elseif actor:hasStatusEffect(xi.effect.MARCATO) then
            return 1.5
        end
    end

    return 1
end

-- Global function to calculate total magicc accuracy.
local function calculateActorMagicAccuracy(actor, target, params)
    local finalMagicAcc = 0

    local magicAccBase       = actor:getMod(xi.mod.MACC) + actor:getILvlMacc(xi.slot.MAIN) + params.bonusMacc
    local magicAccSkill      = magicAccuracyFromSkill(actor, params)
    local magicAccElement    = magicAccuracyFromElement(actor, params)
    local magicAccStatDiff   = magicAccuracyFromStatDifference(actor, target, params)
    local magicAccEffects    = magicAccuracyFromStatusEffects(actor, params)
    local magicAccMerits     = magicAccuracyFromMerits(actor, params)
    local magicAccJobPoints  = magicAccuracyFromJobPoints(actor, params)
    local magicAccBurst      = magicAccuracyFromMagicBurst(target, params)
    local magicAccDayWeather = magicAccuracyFromDayWeatherElement(actor, params)
    local magicAccTandem     = magicAccuracyFromTandemStrike(actor)

    -- Multipliers
    local magicAccFoodFactor      = magicAccuracyFromFoodMultiplier(actor)
    local magicAccSoulVoiceFactor = magicAccuracyFromSoulVoiceMultiplier(actor, params)

    -- Add up food magic accuracy.
    finalMagicAcc = magicAccBase + magicAccSkill + magicAccElement + magicAccStatDiff + magicAccEffects + magicAccMerits + magicAccJobPoints + magicAccBurst + magicAccDayWeather + magicAccTandem
    finalMagicAcc = math.floor(finalMagicAcc * magicAccFoodFactor * magicAccSoulVoiceFactor)

    return finalMagicAcc
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
