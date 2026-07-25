-----------------------------------
-- Ninja Job Utilities
-- Dual-wired pure inject forms:
--   tool resolve/consume (slice 6720 / 0891 → internal/ninjatool)
--   ability products (slice 6750 / 0899 → internal/ninja):
--     one-hour recast, Yonin/Innin/Sange/Futae/Issekigan/Mikage params,
--     Mijin Gakure base + damage product
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
-- Pure inject pins (internal/ninja ability products, slice 6750 / 0899)
-----------------------------------
xi.job_utils.ninja.oneHourRecastSecondsPerMod = 60
xi.job_utils.ninja.yoninPower                 = 30
xi.job_utils.ninja.yoninDurationSec           = 300
xi.job_utils.ninja.yoninTickSec               = 15
xi.job_utils.ninja.inninPower                 = 30
xi.job_utils.ninja.inninDurationSec           = 300
xi.job_utils.ninja.inninTickSec               = 15
xi.job_utils.ninja.inninSubPower              = 20
xi.job_utils.ninja.sangeDurationSec           = 60
xi.job_utils.ninja.sangeMeritOffset           = 1
xi.job_utils.ninja.sangeMeritPowerScale       = 25
xi.job_utils.ninja.futaeDurationSec           = 60
xi.job_utils.ninja.issekiganPower             = 25
xi.job_utils.ninja.issekiganDurationSec       = 60
xi.job_utils.ninja.mikageDurationSec          = 45
xi.job_utils.ninja.mijinGakureHPFraction      = 0.8
xi.job_utils.ninja.mijinGakureJPScale         = 0.03

-- Pure: OneHourRecast
xi.job_utils.ninja.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.ninja.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

-- Pure: Sange power = (merit - 1) * 25
xi.job_utils.ninja.sangePowerFromParams = function(params)
    params = params or {}
    return ((params.merit or 0) - xi.job_utils.ninja.sangeMeritOffset)
        * xi.job_utils.ninja.sangeMeritPowerScale
end

-- Pure: fixed effect params
xi.job_utils.ninja.yoninFromParams = function()
    return {
        power    = xi.job_utils.ninja.yoninPower,
        duration = xi.job_utils.ninja.yoninDurationSec,
        tick     = xi.job_utils.ninja.yoninTickSec,
    }
end

xi.job_utils.ninja.inninFromParams = function()
    return {
        power    = xi.job_utils.ninja.inninPower,
        duration = xi.job_utils.ninja.inninDurationSec,
        tick     = xi.job_utils.ninja.inninTickSec,
        subPower = xi.job_utils.ninja.inninSubPower,
    }
end

xi.job_utils.ninja.sangeFromParams = function(params)
    return {
        power    = xi.job_utils.ninja.sangePowerFromParams(params),
        duration = xi.job_utils.ninja.sangeDurationSec,
    }
end

xi.job_utils.ninja.futaeFromParams = function()
    return {
        duration = xi.job_utils.ninja.futaeDurationSec,
    }
end

xi.job_utils.ninja.issekiganFromParams = function()
    return {
        power    = xi.job_utils.ninja.issekiganPower,
        duration = xi.job_utils.ninja.issekiganDurationSec,
    }
end

xi.job_utils.ninja.mikageFromParams = function()
    return {
        duration = xi.job_utils.ninja.mikageDurationSec,
    }
end

-- Pure: Mijin Gakure base = floor(hp * 0.8)
xi.job_utils.ninja.mijinGakureBaseDamageFromParams = function(params)
    params = params or {}
    return math.floor((params.playerHP or 0) * xi.job_utils.ninja.mijinGakureHPFraction)
end

-- Pure: Mijin Gakure damage product (stoneskin deferred to host)
-- params: base, resist, tmdaFactor, jpLevel
xi.job_utils.ninja.mijinGakureDamageFromParams = function(params)
    params = params or {}
    local dmg = math.floor((params.base or 0) * (params.resist or 0))
    dmg = math.floor(dmg * (params.tmdaFactor or 0))
    local jpFactor = 1 + (params.jpLevel or 0) * xi.job_utils.ninja.mijinGakureJPScale
    dmg = math.floor(dmg * jpFactor)
    return dmg
end

-----------------------------------
-- Ability Check Functions
-----------------------------------

xi.job_utils.ninja.checkMijinGakure = function(player, target, ability)
    ability:setRecast(xi.job_utils.ninja.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
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
    ability:setRecast(xi.job_utils.ninja.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------

xi.job_utils.ninja.useMijinGakure = function(player, target, ability, action)
    local base = xi.job_utils.ninja.mijinGakureBaseDamageFromParams({
        playerHP = player:getHP(),
    })
    local resist     = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, 0, xi.element.NONE, xi.mod.INT, 0, 0)
    local tmdaFactor = xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false)
    local dmg = xi.job_utils.ninja.mijinGakureDamageFromParams({
        base       = base,
        resist     = resist,
        tmdaFactor = tmdaFactor,
        jpLevel    = player:getJobPointLevel(xi.jp.MIJIN_GAKURE_EFFECT),
    })
    dmg = utils.handleStoneskin(target, dmg)

    target:takeDamage(dmg, player, xi.attackType.SPECIAL, xi.damageType.ELEMENTAL)
    player:setLocalVar('MijinGakure', 1)
    player:setHP(0)

    return dmg
end

xi.job_utils.ninja.useYonin = function(player, target, ability, action)
    local p = xi.job_utils.ninja.yoninFromParams()
    target:delStatusEffect(xi.effect.INNIN)
    target:delStatusEffect(xi.effect.YONIN)
    target:addStatusEffect(xi.effect.YONIN, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
    })

    return xi.effect.YONIN
end

xi.job_utils.ninja.useInnin = function(player, target, ability, action)
    local p = xi.job_utils.ninja.inninFromParams()
    target:delStatusEffect(xi.effect.INNIN)
    target:delStatusEffect(xi.effect.YONIN)
    target:addStatusEffect(xi.effect.INNIN, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
        subPower = p.subPower,
    })

    return xi.effect.INNIN
end

xi.job_utils.ninja.useSange = function(player, target, ability, action)
    local p = xi.job_utils.ninja.sangeFromParams({
        merit = player:getMerit(xi.merit.SANGE),
    })
    player:addStatusEffect(xi.effect.SANGE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.SANGE
end

xi.job_utils.ninja.useFutae = function(player, target, ability, action)
    local p = xi.job_utils.ninja.futaeFromParams()
    target:addStatusEffect(xi.effect.FUTAE, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.FUTAE
end

xi.job_utils.ninja.useIssekigan = function(player, target, ability, action)
    local p = xi.job_utils.ninja.issekiganFromParams()
    target:addStatusEffect(xi.effect.ISSEKIGAN, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.ISSEKIGAN
end

xi.job_utils.ninja.useMikage = function(player, target, ability, action)
    local p = xi.job_utils.ninja.mikageFromParams()
    target:addStatusEffect(xi.effect.MIKAGE, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.MIKAGE
end
