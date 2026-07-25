-----------------------------------
-- Global file for skillchain calculations.
-- Pure product injects dual-wired to OmegaXI internal/skillchaindmg (slice 6699 / 0870).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.skillchain = xi.combat.skillchain or {}
-----------------------------------

-- Resistance rank multiplier ladder (clamped ranks −3..11).
xi.combat.skillchain.resistanceRankMultiplier =
{
    [-3] = 1.50,
    [-2] = 1.30,
    [-1] = 1.15,
    [ 0] = 1.00,
    [ 1] = 0.85,
    [ 2] = 0.70,
    [ 3] = 0.60,
    [ 4] = 0.50,
    [ 5] = 0.40,
    [ 6] = 0.30,
    [ 7] = 0.25,
    [ 8] = 0.20,
    [ 9] = 0.15,
    [10] = 0.10,
    [11] = 0.05,
}

-- chainMultipliers[level][count] for level 1..4, count 1..6.
xi.combat.skillchain.chainMultipliers =
{
    [1] = { 0.50, 0.60, 0.70, 0.80, 0.90, 1.00 }, -- Level 1
    [2] = { 0.60, 0.75, 1.00, 1.25, 1.50, 1.75 }, -- Level 2
    [3] = { 1.00, 1.50, 1.75, 2.00, 2.25, 2.50 }, -- Level 3
    [4] = { 1.50, 1.80, 2.10, 2.40, 2.70, 3.00 }, -- Level 4 "Radiance/Umbra"
}

xi.combat.skillchain.minLevel = 1
xi.combat.skillchain.maxLevel = 4
xi.combat.skillchain.minCount = 1
xi.combat.skillchain.maxCount = 6
xi.combat.skillchain.resRankMin = -3
xi.combat.skillchain.resRankMax = 11

-----------------------------------
-- Pure injects
-----------------------------------

xi.combat.skillchain.chainMultiplierFromParams = function(level, count)
    if
        level < xi.combat.skillchain.minLevel or
        level > xi.combat.skillchain.maxLevel or
        count < xi.combat.skillchain.minCount or
        count > xi.combat.skillchain.maxCount
    then
        return 0
    end

    return xi.combat.skillchain.chainMultipliers[level][count]
end

xi.combat.skillchain.clampResRank = function(rank)
    return utils.clamp(rank or 0, xi.combat.skillchain.resRankMin, xi.combat.skillchain.resRankMax)
end

xi.combat.skillchain.resRankMultiplierFromParams = function(rank)
    return xi.combat.skillchain.resistanceRankMultiplier[xi.combat.skillchain.clampResRank(rank)]
end

-- Has magic element on skillchain type (SC power 1..16, element FIRE..DARK).
-- Uses column skillchainType (1-based) matching C++/Go GetSkillchainMagicElement.
-- (Former local host used skillchainType+1, which was off-by-one.)
xi.combat.skillchain.hasElement = function(skillchainType, element)
    if
        not skillchainType or
        skillchainType < 1 or
        skillchainType > 16 or
        not element or
        element < xi.element.FIRE or
        element > xi.element.DARK
    then
        return false
    end

    local row = xi.data.element.skillchainElementTable[element]
    if not row then
        return false
    end

    return (row[skillchainType] or 0) > 0
end

-- Elements for SC type in FIRE→DARK order.
xi.combat.skillchain.elementsFor = function(skillchainType)
    local elementTable = {}

    for i = xi.element.FIRE, xi.element.DARK do
        if xi.combat.skillchain.hasElement(skillchainType, i) then
            table.insert(elementTable, #elementTable + 1, i)
        end
    end

    return elementTable
end

-- Pure getSkillchainElementToUse once res ranks per element are injected.
-- params: skillchainType, resRanks[FIRE..DARK] (raw mod values; missing → 0)
xi.combat.skillchain.selectElementFromParams = function(params)
    local skillchainType = params.skillchainType or 0
    local elements = xi.combat.skillchain.elementsFor(skillchainType)

    if #elements == 0 then
        return 0
    end

    if #elements == 1 then
        return elements[1]
    end

    local ranks = params.resRanks or {}
    local lowestResRank = xi.combat.skillchain.resRankMax
    local lowestElement = xi.element.FIRE

    for j = #elements, 1, -1 do
        local el = elements[j]
        local resRankValue = ranks[el] or 0
        if resRankValue <= lowestResRank then
            lowestResRank = resRankValue
            lowestElement = el
        end
    end

    return lowestElement
end

-- Pure calculateSkillchainDamage product once effect fields and multipliers are injected.
-- params: hasEffect, skillchainType, skillchainLevel, skillchainCount, element, nullified,
--   baseDamage, bonusMult, damageMult, dayWeatherMult, staffMult, affinityMult,
--   magicTakenMult, inninMult, sengikoriMult, absorbMult, resRank, magicDamage
-- returns damage, consumeSengikori, applied, element
xi.combat.skillchain.skillchainDamageProductFromParams = function(params)
    if not params.hasEffect or (params.skillchainType or 0) == 0 then
        return 0, false, false, 0
    end

    local skillchainLevel = params.skillchainLevel or 0
    local skillchainCount = params.skillchainCount or 0
    if
        skillchainLevel < xi.combat.skillchain.minLevel or
        skillchainLevel > xi.combat.skillchain.maxLevel or
        skillchainCount < xi.combat.skillchain.minCount or
        skillchainCount > xi.combat.skillchain.maxCount
    then
        return 0, false, false, 0
    end

    local element = params.element or 0
    if element == 0 then
        return 0, false, false, 0
    end

    if params.nullified then
        -- Nullification short-circuits before product floors and Sengikori reset.
        return 0, false, false, element
    end

    local finalDamage = math.abs(params.baseDamage or 0)
    local levelMultiplier = xi.combat.skillchain.chainMultiplierFromParams(skillchainLevel, skillchainCount)
    local bonusMultiplier = params.bonusMult or 1
    local damageMultiplier = params.damageMult or 1
    local dayWeatherMultiplier = params.dayWeatherMult or 1
    local staffMultiplier = params.staffMult or 1
    local affinityMultiplier = params.affinityMult or 1
    local resRankMultiplier = xi.combat.skillchain.resRankMultiplierFromParams(params.resRank or 0)
    local magicTakenMultiplier = params.magicTakenMult or 1
    local inninMultiplier = params.inninMult or 1
    local sengikoriMultiplier = params.sengikoriMult or 1
    local absorptionMultiplier = params.absorbMult or 1
    local magicDamage = params.magicDamage or 0

    -- Apply multipliers in order and floor after each step.
    finalDamage = math.floor(finalDamage * levelMultiplier)
    finalDamage = math.floor(finalDamage * bonusMultiplier) + magicDamage
    finalDamage = math.floor(finalDamage * damageMultiplier)
    finalDamage = math.floor(finalDamage * dayWeatherMultiplier)
    finalDamage = math.floor(finalDamage * staffMultiplier)
    finalDamage = math.floor(finalDamage * affinityMultiplier)
    finalDamage = math.floor(finalDamage * resRankMultiplier)
    finalDamage = math.floor(finalDamage * magicTakenMultiplier)
    finalDamage = math.floor(finalDamage * inninMultiplier)
    finalDamage = math.floor(finalDamage * sengikoriMultiplier)
    finalDamage = math.floor(finalDamage * absorptionMultiplier)

    return finalDamage, true, true, element
end

-----------------------------------
-- Entity hosts (inject → pure + side effects)
-----------------------------------

-- Select SC magic element for target (lowest resistance among SC elements).
local function getSkillchainElementToUse(target, skillchainType)
    local resRanks = {}

    for i = xi.element.FIRE, xi.element.DARK do
        resRanks[i] = target:getMod(xi.data.element.getElementalResistanceRankModifier(i))
    end

    return xi.combat.skillchain.selectElementFromParams({
        skillchainType = skillchainType,
        resRanks       = resRanks,
    })
end

-- Handles skillchain magic damage multipliers, called from C++ (battleutils::TakeSkillchainDamage)
xi.combat.skillchain.calculateSkillchainDamage = function(actor, target, baseDamage)
    local skillchainEffect = target:getStatusEffect(xi.effect.SKILLCHAIN)
    if not skillchainEffect then
        return 0
    end

    local skillchainType = skillchainEffect:getPower()
    if skillchainType == 0 then
        return 0
    end

    local skillchainLevel = skillchainEffect:getTier()
    local skillchainCount = skillchainEffect:getSubPower()

    local skillchainElement = getSkillchainElementToUse(target, skillchainType)
    if not skillchainElement or skillchainElement == 0 then
        return 0
    end

    local nullified = xi.spells.damage.calculateNullification(target, skillchainElement, true, false) == 0

    -- Resistance rank for product (clamped inside pure).
    local resRankModifier = xi.data.element.getElementalResistanceRankModifier(skillchainElement)
    local resRankValue = target:getMod(resRankModifier)

    local finalDamage, consumeSengikori = xi.combat.skillchain.skillchainDamageProductFromParams({
        hasEffect        = true,
        skillchainType   = skillchainType,
        skillchainLevel  = skillchainLevel,
        skillchainCount  = skillchainCount,
        element          = skillchainElement,
        nullified        = nullified,
        baseDamage       = baseDamage,
        bonusMult        = 1 + actor:getMod(xi.mod.SKILLCHAINBONUS) / 100,
        damageMult       = 1 + actor:getMod(xi.mod.SKILLCHAINDMG) / 10000,
        dayWeatherMult   = xi.spells.damage.calculateDayAndWeather(actor, skillchainElement, false),
        staffMult        = xi.spells.damage.calculateElementalStaffBonus(actor, skillchainElement),
        affinityMult     = xi.spells.damage.calculateElementalAffinityBonus(actor, skillchainElement),
        magicTakenMult   = xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false),
        inninMult        = 1 + actor:getMerit(xi.merit.INNIN_EFFECT) / 100,
        sengikoriMult    = 1 + target:getMod(xi.mod.SENGIKORI_SC_DMG_DEBUFF) / 100,
        absorbMult       = xi.spells.damage.calculateAbsorption(target, skillchainElement, true),
        resRank          = resRankValue,
        magicDamage      = actor:getMod(xi.mod.MAGIC_DAMAGE),
    })

    -- Handle (reset) Sengikori when product path ran.
    if consumeSengikori then
        target:setMod(xi.mod.SENGIKORI_SC_DMG_DEBUFF, 0)
    end

    if finalDamage == 0 and not consumeSengikori then
        return 0
    end

    -- Handle other damage alterations.
    if finalDamage > 0 then
        finalDamage = utils.clamp(utils.handlePhalanx(target, finalDamage), 0, 99999)
        finalDamage = utils.clamp(utils.handleOneForAll(target, finalDamage), 0, 99999)
        finalDamage = utils.clamp(utils.handleStoneskin(target, finalDamage), 0, 99999)
        finalDamage = target:checkDamageCap(finalDamage)

        target:takeDamage(finalDamage, actor, xi.attackType.SPECIAL, xi.damageType.ELEMENTAL + skillchainElement)

    -- Handle absorbption.
    else
        target:addHP(-finalDamage)
    end

    return finalDamage
end
