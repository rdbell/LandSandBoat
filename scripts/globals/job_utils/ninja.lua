-----------------------------------
-- Ninja Job Utilities
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.ninja = xi.job_utils.ninja or {}

-----------------------------------
-- Ninja tool pure helpers
-- Dual-wired to OmegaXI internal/ninjatool (slice 6720 / 0891).
-- Parity: battleutils::HasNinjaTool / ninja_tool_capacity.h
-----------------------------------

xi.job_utils.ninja.tool =
{
    UCHITAKE         = 1161,
    TSURARA          = 1164,
    KAWAHORI_OGI     = 1167,
    MAKIBISHI        = 1170,
    HIRAISHIN        = 1173,
    MIZU_DEPPO       = 1176,
    SHIHEI           = 1179,
    JUSATSU          = 1182,
    KAGINAWA         = 1185,
    SAIRUI_RAN       = 1188,
    KODOKU           = 1191,
    SHINOBI_TABI     = 1194,
    SANJAKU_TENUGUI  = 2553,
    SOSHI            = 2555,
    KABENRO          = 2642,
    JINKO            = 2643,
    RYUNO            = 2644,
    MOKUJIN          = 2970,
    INOSHISHINOFUDA  = 2971, -- elemental-wheel universal
    SHIKANOFUDA      = 2972, -- toolbag / utility universal
    CHONOFUDA        = 2973, -- enfeeble universal
    RANKA            = 8803,
    FURUSUMI         = 8804,
}

-- Preferred tool → NIN main-job universal substitute. Returns id or nil.
xi.job_utils.ninja.substituteTool = function(preferred)
    preferred = preferred or 0
    local t = xi.job_utils.ninja.tool

    if
        preferred == t.UCHITAKE or preferred == t.TSURARA or preferred == t.KAWAHORI_OGI or
        preferred == t.MAKIBISHI or preferred == t.HIRAISHIN or preferred == t.MIZU_DEPPO
    then
        return t.INOSHISHINOFUDA
    elseif
        preferred == t.RYUNO or preferred == t.MOKUJIN or preferred == t.SANJAKU_TENUGUI or
        preferred == t.KABENRO or preferred == t.SHINOBI_TABI or preferred == t.SHIHEI or
        preferred == t.RANKA or preferred == t.FURUSUMI
    then
        return t.SHIKANOFUDA
    elseif
        preferred == t.SOSHI or preferred == t.KODOKU or preferred == t.KAGINAWA or
        preferred == t.JUSATSU or preferred == t.SAIRUI_RAN or preferred == t.JINKO
    then
        return t.CHONOFUDA
    end

    return nil
end

-- Six primary elemental tools that Futae can consume two of.
xi.job_utils.ninja.isElementalWheelTool = function(toolId)
    toolId = toolId or 0
    local t = xi.job_utils.ninja.tool
    return toolId == t.UCHITAKE or toolId == t.TSURARA or toolId == t.KAWAHORI_OGI or
        toolId == t.MAKIBISHI or toolId == t.HIRAISHIN or toolId == t.MIZU_DEPPO
end

-- Resolve preferred tool with optional NIN main-job substitute after miss.
-- params: preferred, preferredAvailable, isNINMain, substituteAvailable
-- returns: { toolId, ok, usedSubstitute }
xi.job_utils.ninja.resolveNinjaToolFromParams = function(params)
    local preferred = params.preferred or 0
    if params.preferredAvailable then
        return { toolId = preferred, ok = true, usedSubstitute = false }
    end

    if not params.isNINMain then
        return { toolId = 0, ok = false, usedSubstitute = false }
    end

    local sub = xi.job_utils.ninja.substituteTool(preferred)
    if not sub or not params.substituteAvailable then
        return { toolId = 0, ok = false, usedSubstitute = false }
    end

    return { toolId = sub, ok = true, usedSubstitute = true }
end

-- Expertise chance = NINJA_TOOL mod + merit bonus (0 when trait absent).
xi.job_utils.ninja.expertiseChance = function(ninjaToolMod, meritBonus)
    return (ninjaToolMod or 0) + (meritBonus or 0)
end

-- GetRandomNumber(100) roll 0..99; consume when roll > chance.
xi.job_utils.ninja.shouldConsumeTool = function(chance, roll)
    return (roll or 0) > (chance or 0)
end

-- Inventory delta when ConsumeTool is true.
-- Futae + elemental wheel → 2; else expertise path → 1 or 0.
xi.job_utils.ninja.consumeToolQty = function(toolId, hasFutae, expertiseChance, roll)
    if hasFutae and xi.job_utils.ninja.isElementalWheelTool(toolId) then
        return 2
    end

    if xi.job_utils.ninja.shouldConsumeTool(expertiseChance, roll) then
        return 1
    end

    return 0
end

-- Non-PC entities always pass HasNinjaTool without inventory checks.
xi.job_utils.ninja.nonPCAlwaysHasTool = function()
    return true
end

-----------------------------------
-- Ability Check Functions
-----------------------------------

xi.job_utils.ninja.checkMijinGakure = function(player, target, ability)
    ability:setRecast(math.max(0, ability:getRecast() - player:getMod(xi.mod.ONE_HOUR_RECAST) * 60))
    return 0, 0
end

xi.job_utils.ninja.checkYonin = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ninja.checkInnin = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ninja.checkSange = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ninja.checkFutae = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ninja.checkIssekigan = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ninja.checkMikage = function(player, target, ability)
    ability:setRecast(math.max(0, ability:getRecast() - player:getMod(xi.mod.ONE_HOUR_RECAST) * 60))
    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------

xi.job_utils.ninja.useMijinGakure = function(player, target, ability, action)
    local dmg        = math.floor(player:getHP() * 0.8)
    local resist     = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, 0, xi.element.NONE, xi.mod.INT, 0, 0)
    local tmdaFactor = xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false)
    local jpFactor   = 1 + player:getJobPointLevel(xi.jp.MIJIN_GAKURE_EFFECT) * 0.03

    dmg = math.floor(dmg * resist)
    dmg = math.floor(dmg * tmdaFactor)
    dmg = math.floor(dmg * jpFactor)
    dmg = utils.handleStoneskin(target, dmg)

    target:takeDamage(dmg, player, xi.attackType.SPECIAL, xi.damageType.ELEMENTAL)
    player:setLocalVar('MijinGakure', 1)
    player:setHP(0)

    return dmg
end

xi.job_utils.ninja.useYonin = function(player, target, ability, action)
    target:delStatusEffect(xi.effect.INNIN)
    target:delStatusEffect(xi.effect.YONIN)
    target:addStatusEffect(xi.effect.YONIN, { power = 30, duration = 300, origin = player, tick = 15 })

    return xi.effect.YONIN
end

xi.job_utils.ninja.useInnin = function(player, target, ability, action)
    target:delStatusEffect(xi.effect.INNIN)
    target:delStatusEffect(xi.effect.YONIN)
    target:addStatusEffect(xi.effect.INNIN, { power = 30, duration = 300, origin = player, tick = 15, subPower = 20 })

    return xi.effect.INNIN
end

xi.job_utils.ninja.useSange = function(player, target, ability, action)
    local potency = player:getMerit(xi.merit.SANGE)-1
    player:addStatusEffect(xi.effect.SANGE, { power = potency * 25, duration = 60, origin = player })

    return xi.effect.SANGE
end

xi.job_utils.ninja.useFutae = function(player, target, ability, action)
    target:addStatusEffect(xi.effect.FUTAE, { duration = 60, origin = player })

    return xi.effect.FUTAE
end

xi.job_utils.ninja.useIssekigan = function(player, target, ability, action)
    target:addStatusEffect(xi.effect.ISSEKIGAN, { power = 25, duration = 60, origin = player })

    return xi.effect.ISSEKIGAN
end

xi.job_utils.ninja.useMikage = function(player, target, ability, action)
    target:addStatusEffect(xi.effect.MIKAGE, { duration = 45, origin = player })

    return xi.effect.MIKAGE
end
