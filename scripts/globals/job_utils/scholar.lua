-----------------------------------
-- Scholar Job Utilities
-- Dual-wired pure inject forms (slice 6749 / 5975 / 5978):
--   already-active gates, fixed/merit stratagems, Enlightenment,
--   Light/Dark Arts + Addenda, Helix/Regen bonuses, Tabula Rasa,
--   Sublimation, Modus Veritas, Caper Emissarius, OneHourRecast
-- Parity: internal/scholar
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.scholar = xi.job_utils.scholar or {}

-----------------------------------
-- Pure inject pins (internal/scholar)
-----------------------------------
xi.job_utils.scholar.stratagemBasePower            = 1
xi.job_utils.scholar.stratagemDurationSec          = 60
xi.job_utils.scholar.artsDurationSec               = 7200
xi.job_utils.scholar.darkArtsBasePower             = 1
xi.job_utils.scholar.tabulaRasaDurationSec         = 180
xi.job_utils.scholar.tabulaRasaBonusScale          = 1.5
xi.job_utils.scholar.tabulaRasaJPMPFraction        = 0.02
xi.job_utils.scholar.sublimationChargeDurationSec  = 7200
xi.job_utils.scholar.sublimationChargeTick         = 3
xi.job_utils.scholar.sublimationRefreshTierBlock   = 3
xi.job_utils.scholar.artsBonusLevelFloor           = 20
xi.job_utils.scholar.enlightenmentMeritOffset      = 5
xi.job_utils.scholar.modusVeritasBaseMultiplier    = 0.5
xi.job_utils.scholar.modusVeritasMeritStep         = 0.05
xi.job_utils.scholar.modusVeritasJPPower           = 3
xi.job_utils.scholar.modusVeritasResistFloor       = 0.25
xi.job_utils.scholar.oneHourRecastSecondsPerMod    = 60
xi.job_utils.scholar.caperEnmityPercent            = 99
xi.job_utils.scholar.caperEnmityRange              = 20.6
xi.job_utils.scholar.msgEffectAlreadyActive        = 523
xi.job_utils.scholar.msgJAMiss                     = 158
xi.job_utils.scholar.msgJANoEffect2                = 323
xi.job_utils.scholar.msgJARecoversMP               = 451
xi.job_utils.scholar.msgCannotOnThatTarg           = 155

-- Tabula Rasa reset recast IDs (literal pins)
xi.job_utils.scholar.tabulaRasaResetRecastIDs = { 228, 231, 232 }

-- Sublimation plan kinds (mirror Go SublimationKind)
xi.job_utils.scholar.sublimationRecoverComplete   = 0
xi.job_utils.scholar.sublimationRecoverActivated  = 1
xi.job_utils.scholar.sublimationStartCharge       = 2
xi.job_utils.scholar.sublimationBlockedByRefresh  = 3

-- Modus Veritas outcomes
xi.job_utils.scholar.modusVeritasNoEffect = 0
xi.job_utils.scholar.modusVeritasMiss     = 1
xi.job_utils.scholar.modusVeritasApply    = 2

-----------------------------------
-- Pure: already-active check
-- params: hasEffect
-- returns: msgId, param
-----------------------------------
xi.job_utils.scholar.checkAlreadyActiveFromParams = function(params)
    params = params or {}
    if params.hasEffect then
        return xi.job_utils.scholar.msgEffectAlreadyActive, 0
    end

    return 0, 0
end

-- Pure: Light Arts dual gate
-- params: hasLightArts, hasAddendumWhite
xi.job_utils.scholar.checkLightArtsFromParams = function(params)
    params = params or {}
    if params.hasLightArts or params.hasAddendumWhite then
        return xi.job_utils.scholar.msgEffectAlreadyActive, 0
    end

    return 0, 0
end

-- Pure: Dark Arts dual gate
-- params: hasDarkArts, hasAddendumBlack
xi.job_utils.scholar.checkDarkArtsFromParams = function(params)
    params = params or {}
    if params.hasDarkArts or params.hasAddendumBlack then
        return xi.job_utils.scholar.msgEffectAlreadyActive, 0
    end

    return 0, 0
end

-- Pure: fixed stratagem params { power=1, duration=60 }
xi.job_utils.scholar.stratagemParamsFromParams = function()
    return {
        power    = xi.job_utils.scholar.stratagemBasePower,
        duration = xi.job_utils.scholar.stratagemDurationSec,
    }
end

-- Pure: merit stratagem params
-- params: merit
xi.job_utils.scholar.meritStratagemParamsFromParams = function(params)
    params = params or {}
    return {
        power    = params.merit or 0,
        duration = xi.job_utils.scholar.stratagemDurationSec,
    }
end

-- Pure: Enlightenment power = merit - 5
xi.job_utils.scholar.enlightenmentPowerFromParams = function(params)
    params = params or {}
    return (params.enlightenmentMerit or 0) - xi.job_utils.scholar.enlightenmentMeritOffset
end

xi.job_utils.scholar.enlightenmentParamsFromParams = function(params)
    return {
        power    = xi.job_utils.scholar.enlightenmentPowerFromParams(params),
        duration = xi.job_utils.scholar.stratagemDurationSec,
    }
end

-- Pure: HelixBonus — main SCH >= 20 → floor(mainLvl / 4)
xi.job_utils.scholar.helixBonusFromParams = function(params)
    params = params or {}
    if not params.mainJobIsSCH or (params.mainLevel or 0) < xi.job_utils.scholar.artsBonusLevelFloor then
        return 0
    end

    return math.floor((params.mainLevel or 0) / 4)
end

-- Pure: RegenBonus — main SCH >= 20 → 3 * floor((mainLvl - 10) / 10)
xi.job_utils.scholar.regenBonusFromParams = function(params)
    params = params or {}
    if not params.mainJobIsSCH or (params.mainLevel or 0) < xi.job_utils.scholar.artsBonusLevelFloor then
        return 0
    end

    return 3 * math.floor(((params.mainLevel or 0) - 10) / 10)
end

-- Pure: Light Arts plan
-- params: lightArtsEffectMod, mainJobIsSCH, mainLevel
-- returns: effectId, power, duration, subPower, silentDel, del
xi.job_utils.scholar.lightArtsPlanFromParams = function(params)
    params = params or {}
    local subPower = xi.job_utils.scholar.regenBonusFromParams(params)
    return {
        effect    = xi.effect.LIGHT_ARTS,
        power     = params.lightArtsEffectMod or 0,
        duration  = xi.job_utils.scholar.artsDurationSec,
        subPower  = subPower,
        silentDel = { xi.effect.DARK_ARTS },
        del       = {
            xi.effect.ADDENDUM_BLACK,
            xi.effect.PARSIMONY,
            xi.effect.ALACRITY,
            xi.effect.MANIFESTATION,
            xi.effect.EBULLIENCE,
            xi.effect.FOCALIZATION,
            xi.effect.EQUANIMITY,
            xi.effect.IMMANENCE,
        },
    }
end

-- Pure: Dark Arts plan
xi.job_utils.scholar.darkArtsPlanFromParams = function(params)
    params = params or {}
    local subPower = xi.job_utils.scholar.helixBonusFromParams(params)
    return {
        effect    = xi.effect.DARK_ARTS,
        power     = xi.job_utils.scholar.darkArtsBasePower,
        duration  = xi.job_utils.scholar.artsDurationSec,
        subPower  = subPower,
        silentDel = { xi.effect.LIGHT_ARTS },
        del       = {
            xi.effect.ADDENDUM_WHITE,
            xi.effect.PENURY,
            xi.effect.CELERITY,
            xi.effect.ACCESSION,
            xi.effect.RAPTURE,
            xi.effect.ALTRUISM,
            xi.effect.TRANQUILITY,
            xi.effect.PERPETUANCE,
        },
    }
end

-- Pure: Addendum White plan (silent add)
xi.job_utils.scholar.addendumWhitePlanFromParams = function(params)
    params = params or {}
    return {
        effect    = xi.effect.ADDENDUM_WHITE,
        power     = params.lightArtsEffectMod or 0,
        duration  = xi.job_utils.scholar.artsDurationSec,
        subPower  = xi.job_utils.scholar.regenBonusFromParams(params),
        silent    = true,
        silentDel = {
            xi.effect.DARK_ARTS,
            xi.effect.ADDENDUM_BLACK,
            xi.effect.LIGHT_ARTS,
        },
        del       = {},
    }
end

-- Pure: Addendum Black plan (silent add)
xi.job_utils.scholar.addendumBlackPlanFromParams = function(params)
    params = params or {}
    return {
        effect    = xi.effect.ADDENDUM_BLACK,
        power     = params.darkArtsEffectMod or 0,
        duration  = xi.job_utils.scholar.artsDurationSec,
        subPower  = xi.job_utils.scholar.helixBonusFromParams(params),
        silent    = true,
        silentDel = {
            xi.effect.LIGHT_ARTS,
            xi.effect.ADDENDUM_WHITE,
            xi.effect.DARK_ARTS,
        },
        del       = {},
    }
end

-- Pure: OneHourRecast
xi.job_utils.scholar.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.scholar.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

-- Pure: Tabula Rasa JP MP restore
xi.job_utils.scholar.tabulaRasaJPMPRestoreFromParams = function(params)
    params = params or {}
    local jp = params.jpLevel or 0
    if jp <= 0 then
        return 0
    end

    return math.floor((params.maxMP or 0) * xi.job_utils.scholar.tabulaRasaJPMPFraction * jp)
end

-- Pure: PlanTabulaRasa
-- returns: power, duration, subPower, jpMPRestore, resetRecastIDs
xi.job_utils.scholar.planTabulaRasaFromParams = function(params)
    params = params or {}
    local helix = xi.job_utils.scholar.helixBonusFromParams(params)
    local regen = xi.job_utils.scholar.regenBonusFromParams(params)
    local scale = xi.job_utils.scholar.tabulaRasaBonusScale
    return {
        power          = math.floor(helix * scale),
        duration       = xi.job_utils.scholar.tabulaRasaDurationSec,
        subPower       = math.floor(regen * scale),
        jpMPRestore    = xi.job_utils.scholar.tabulaRasaJPMPRestoreFromParams(params),
        resetRecastIDs = {
            xi.job_utils.scholar.tabulaRasaResetRecastIDs[1],
            xi.job_utils.scholar.tabulaRasaResetRecastIDs[2],
            xi.job_utils.scholar.tabulaRasaResetRecastIDs[3],
        },
    }
end

local function clampMPRecover(stored, currentMP, maxMP)
    local mp = stored or 0
    if mp + currentMP > maxMP then
        mp = maxMP - currentMP
    end

    if mp < 0 then
        return 0
    end

    return mp
end

-- Pure: PlanSublimation
-- params: hasComplete, hasActivated, storedPower, currentMP, maxMP, refreshTier
-- returns plan table with kind, mpRecover, msgId, clearEffect, delRefresh, chargeDuration, chargeTick, returnMPAmount
xi.job_utils.scholar.planSublimationFromParams = function(params)
    params = params or {}
    local currentMP = params.currentMP or 0
    local maxMP     = params.maxMP or 0
    local stored    = params.storedPower or 0

    if params.hasComplete then
        local mp = clampMPRecover(stored, currentMP, maxMP)
        return {
            kind           = xi.job_utils.scholar.sublimationRecoverComplete,
            mpRecover      = mp,
            msgId          = xi.job_utils.scholar.msgJARecoversMP,
            clearEffect    = xi.effect.SUBLIMATION_COMPLETE,
            delRefresh     = false,
            returnMPAmount = mp,
        }
    end

    if params.hasActivated then
        local mp = clampMPRecover(stored, currentMP, maxMP)
        return {
            kind           = xi.job_utils.scholar.sublimationRecoverActivated,
            mpRecover      = mp,
            msgId          = xi.job_utils.scholar.msgJARecoversMP,
            clearEffect    = xi.effect.SUBLIMATION_ACTIVATED,
            delRefresh     = false,
            returnMPAmount = mp,
        }
    end

    if (params.refreshTier or 0) < xi.job_utils.scholar.sublimationRefreshTierBlock then
        return {
            kind           = xi.job_utils.scholar.sublimationStartCharge,
            mpRecover      = 0,
            msgId          = 0,
            clearEffect    = 0,
            delRefresh     = true,
            chargeDuration = xi.job_utils.scholar.sublimationChargeDurationSec,
            chargeTick     = xi.job_utils.scholar.sublimationChargeTick,
            returnMPAmount = 0,
        }
    end

    return {
        kind           = xi.job_utils.scholar.sublimationBlockedByRefresh,
        mpRecover      = 0,
        msgId          = xi.job_utils.scholar.msgJANoEffect2,
        clearEffect    = 0,
        delRefresh     = false,
        returnMPAmount = 0,
    }
end

-- Pure: PlanModusVeritas
-- params: hasHelix, helixSubPower, helixPower, helixDurationSec, remainingSec,
--         resist, isNM, modusVeritasMerit, modusVeritasJP
xi.job_utils.scholar.planModusVeritasFromParams = function(params)
    params = params or {}
    if not params.hasHelix then
        return {
            outcome        = xi.job_utils.scholar.modusVeritasNoEffect,
            msgId          = xi.job_utils.scholar.msgJANoEffect2,
            returnValue    = 0,
            newSubPower    = 0,
            newHelixPower  = 0,
            newDurationSec = 0,
            newDurationMs  = 0,
        }
    end

    local subPower = params.helixSubPower or 0
    local resist   = params.resist or 0
    if
        subPower > 0 or
        resist < xi.job_utils.scholar.modusVeritasResistFloor or
        params.isNM
    then
        return {
            outcome        = xi.job_utils.scholar.modusVeritasMiss,
            msgId          = xi.job_utils.scholar.msgJAMiss,
            returnValue    = 0,
            newSubPower    = 0,
            newHelixPower  = 0,
            newDurationSec = 0,
            newDurationMs  = 0,
        }
    end

    local durationMultiplier = xi.job_utils.scholar.modusVeritasBaseMultiplier
        + xi.job_utils.scholar.modusVeritasMeritStep * (params.modusVeritasMerit or 0)
    local newSub = subPower + 1
    local newPower = (params.helixPower or 0) * 2
        + xi.job_utils.scholar.modusVeritasJPPower * (params.modusVeritasJP or 0)
    local helixDurationSec = params.helixDurationSec or 0
    local remainingSec     = params.remainingSec or 0
    local elapsed          = helixDurationSec - remainingSec
    local scaledRemain     = math.floor(remainingSec * durationMultiplier)
    local newDurSec        = elapsed + scaledRemain
    return {
        outcome        = xi.job_utils.scholar.modusVeritasApply,
        msgId          = 0,
        returnValue    = 0,
        newSubPower    = newSub,
        newHelixPower  = newPower,
        newDurationSec = newDurSec,
        newDurationMs  = newDurSec * 1000,
    }
end

-- Pure: Caper Emissarius check
-- params: hasTarget, isPC, sameAsActor
xi.job_utils.scholar.checkCaperEmissariusFromParams = function(params)
    params = params or {}
    if not params.hasTarget or params.sameAsActor or not params.isPC then
        return xi.job_utils.scholar.msgCannotOnThatTarg, 0
    end

    return 0, 0
end

-- Pure: Caper transfer inject pair
xi.job_utils.scholar.caperTransferParamsFromParams = function()
    return xi.job_utils.scholar.caperEnmityPercent, xi.job_utils.scholar.caperEnmityRange
end

-- Pure: Libra is a no-op upstream
xi.job_utils.scholar.libraIsImplementedFromParams = function()
    return false
end

-----------------------------------
-- Ability Check / Use hosts (inject → pure)
-----------------------------------
xi.job_utils.scholar.checkAlreadyActiveEffect = function(player, effectId)
    return xi.job_utils.scholar.checkAlreadyActiveFromParams({
        hasEffect = player:hasStatusEffect(effectId),
    })
end

xi.job_utils.scholar.useFixedStratagem = function(player, effectId)
    local p = xi.job_utils.scholar.stratagemParamsFromParams()
    player:addStatusEffect(effectId, { power = p.power, duration = p.duration, origin = player })
    return effectId
end

xi.job_utils.scholar.useMeritStratagem = function(player, effectId, meritId)
    local p = xi.job_utils.scholar.meritStratagemParamsFromParams({
        merit = player:getMerit(meritId),
    })
    player:addStatusEffect(effectId, { power = p.power, duration = p.duration, origin = player })
    return effectId
end

xi.job_utils.scholar.checkLightArts = function(player, target, ability)
    return xi.job_utils.scholar.checkLightArtsFromParams({
        hasLightArts     = player:hasStatusEffect(xi.effect.LIGHT_ARTS),
        hasAddendumWhite = player:hasStatusEffect(xi.effect.ADDENDUM_WHITE),
    })
end

xi.job_utils.scholar.useLightArts = function(player, target, ability)
    local plan = xi.job_utils.scholar.lightArtsPlanFromParams({
        lightArtsEffectMod = player:getMod(xi.mod.LIGHT_ARTS_EFFECT),
        mainJobIsSCH       = player:getMainJob() == xi.job.SCH,
        mainLevel          = player:getMainLvl(),
    })
    for _, effectId in ipairs(plan.silentDel) do
        player:delStatusEffectSilent(effectId)
    end

    for _, effectId in ipairs(plan.del) do
        player:delStatusEffect(effectId)
    end

    player:addStatusEffect(plan.effect, {
        power    = plan.power,
        duration = plan.duration,
        origin   = player,
        subPower = plan.subPower,
    })
    return plan.effect
end

xi.job_utils.scholar.checkDarkArts = function(player, target, ability)
    return xi.job_utils.scholar.checkDarkArtsFromParams({
        hasDarkArts      = player:hasStatusEffect(xi.effect.DARK_ARTS),
        hasAddendumBlack = player:hasStatusEffect(xi.effect.ADDENDUM_BLACK),
    })
end

xi.job_utils.scholar.useDarkArts = function(player, target, ability)
    local plan = xi.job_utils.scholar.darkArtsPlanFromParams({
        mainJobIsSCH = player:getMainJob() == xi.job.SCH,
        mainLevel    = player:getMainLvl(),
    })
    for _, effectId in ipairs(plan.silentDel) do
        player:delStatusEffectSilent(effectId)
    end

    for _, effectId in ipairs(plan.del) do
        player:delStatusEffect(effectId)
    end

    player:addStatusEffect(plan.effect, {
        power    = plan.power,
        duration = plan.duration,
        origin   = player,
        subPower = plan.subPower,
    })
    return plan.effect
end

xi.job_utils.scholar.checkAddendumWhite = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveFromParams({
        hasEffect = player:hasStatusEffect(xi.effect.ADDENDUM_WHITE),
    })
end

xi.job_utils.scholar.useAddendumWhite = function(player, target, ability)
    local plan = xi.job_utils.scholar.addendumWhitePlanFromParams({
        lightArtsEffectMod = player:getMod(xi.mod.LIGHT_ARTS_EFFECT),
        mainJobIsSCH       = player:getMainJob() == xi.job.SCH,
        mainLevel          = player:getMainLvl(),
    })
    for _, effectId in ipairs(plan.silentDel) do
        player:delStatusEffectSilent(effectId)
    end

    player:addStatusEffect(plan.effect, {
        power    = plan.power,
        duration = plan.duration,
        origin   = player,
        subPower = plan.subPower,
        silent   = plan.silent,
    })
    return plan.effect
end

xi.job_utils.scholar.checkAddendumBlack = function(player, target, ability)
    return xi.job_utils.scholar.checkAlreadyActiveFromParams({
        hasEffect = player:hasStatusEffect(xi.effect.ADDENDUM_BLACK),
    })
end

xi.job_utils.scholar.useAddendumBlack = function(player, target, ability)
    local plan = xi.job_utils.scholar.addendumBlackPlanFromParams({
        darkArtsEffectMod = player:getMod(xi.mod.DARK_ARTS_EFFECT),
        mainJobIsSCH      = player:getMainJob() == xi.job.SCH,
        mainLevel         = player:getMainLvl(),
    })
    for _, effectId in ipairs(plan.silentDel) do
        player:delStatusEffectSilent(effectId)
    end

    player:addStatusEffect(plan.effect, {
        power    = plan.power,
        duration = plan.duration,
        origin   = player,
        subPower = plan.subPower,
        silent   = plan.silent,
    })
    return plan.effect
end

xi.job_utils.scholar.checkTabulaRasa = function(player, target, ability)
    ability:setRecast(xi.job_utils.scholar.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.scholar.useTabulaRasa = function(player, target, ability)
    local plan = xi.job_utils.scholar.planTabulaRasaFromParams({
        mainJobIsSCH = player:getMainJob() == xi.job.SCH,
        mainLevel    = player:getMainLvl(),
        maxMP        = player:getMaxMP(),
        jpLevel      = player:getJobPointLevel(xi.jp.TABULA_RASA_EFFECT),
    })
    if plan.jpMPRestore > 0 then
        player:addMP(plan.jpMPRestore)
    end

    for _, recastId in ipairs(plan.resetRecastIDs) do
        player:resetRecast(xi.recast.ABILITY, recastId)
    end

    player:addStatusEffect(xi.effect.TABULA_RASA, {
        power    = plan.power,
        duration = plan.duration,
        origin   = player,
        subPower = plan.subPower,
    })
    return xi.effect.TABULA_RASA
end

xi.job_utils.scholar.useSublimation = function(player, target, ability)
    local hasComplete  = player:hasStatusEffect(xi.effect.SUBLIMATION_COMPLETE)
    local hasActivated = player:hasStatusEffect(xi.effect.SUBLIMATION_ACTIVATED)
    local storedPower  = 0
    if hasComplete then
        storedPower = player:getStatusEffect(xi.effect.SUBLIMATION_COMPLETE):getPower()
    elseif hasActivated then
        storedPower = player:getStatusEffect(xi.effect.SUBLIMATION_ACTIVATED):getPower()
    end

    local refreshTier = player:hasStatusEffect(xi.effect.REFRESH)
        and player:getStatusEffect(xi.effect.REFRESH):getTier()
        or 0

    local plan = xi.job_utils.scholar.planSublimationFromParams({
        hasComplete  = hasComplete,
        hasActivated = hasActivated,
        storedPower  = storedPower,
        currentMP    = player:getMP(),
        maxMP        = player:getMaxMP(),
        refreshTier  = refreshTier,
    })

    if
        plan.kind == xi.job_utils.scholar.sublimationRecoverComplete or
        plan.kind == xi.job_utils.scholar.sublimationRecoverActivated
    then
        player:addMP(plan.mpRecover)
        player:delStatusEffectSilent(plan.clearEffect)
        ability:setMsg(plan.msgId)
        return plan.returnMPAmount
    end

    if plan.kind == xi.job_utils.scholar.sublimationStartCharge then
        if plan.delRefresh then
            player:delStatusEffect(xi.effect.REFRESH)
        end

        player:addStatusEffect(xi.effect.SUBLIMATION_ACTIVATED, {
            duration = plan.chargeDuration,
            origin   = player,
            tick     = plan.chargeTick,
        })
        return plan.returnMPAmount
    end

    -- blocked by refresh
    ability:setMsg(plan.msgId)
    return plan.returnMPAmount
end

xi.job_utils.scholar.useModusVeritas = function(player, target, ability)
    local helix = target:getStatusEffect(xi.effect.HELIX)
    local hasHelix = helix ~= nil

    -- No helix → pure no-effect path (avoid entity injects not needed).
    if not hasHelix then
        local plan = xi.job_utils.scholar.planModusVeritasFromParams({ hasHelix = false })
        ability:setMsg(plan.msgId)
        return
    end

    local helixSubPower    = helix:getSubPower()
    local helixPower       = helix:getPower()
    local helixDurationSec = helix:getDuration()
    local remainingSec     = math.floor(helix:getTimeRemaining() / 1000)
    local resist = xi.combat.magicHitRate.calculateResistRate(
        player, target, 0, xi.skill.ELEMENTAL_MAGIC, 0, xi.element.NONE, 0, 0, 0)

    local plan = xi.job_utils.scholar.planModusVeritasFromParams({
        hasHelix           = true,
        helixSubPower      = helixSubPower,
        helixPower         = helixPower,
        helixDurationSec   = helixDurationSec,
        remainingSec       = remainingSec,
        resist             = resist,
        isNM               = target:isNM(),
        modusVeritasMerit  = player:getMerit(xi.merit.MODUS_VERITAS_DURATION),
        modusVeritasJP     = player:getJobPointLevel(xi.jp.MODUS_VERITAS_EFFECT),
    })

    if plan.outcome == xi.job_utils.scholar.modusVeritasMiss then
        ability:setMsg(plan.msgId)
        return plan.returnValue
    end

    -- apply
    helix:setSubPower(plan.newSubPower)
    helix:setPower(plan.newHelixPower)
    helix:setDuration(plan.newDurationMs)
end

xi.job_utils.scholar.useEnlightenment = function(player, target, ability)
    local p = xi.job_utils.scholar.enlightenmentParamsFromParams({
        enlightenmentMerit = player:getMerit(xi.merit.ENLIGHTENMENT),
    })
    player:addStatusEffect(xi.effect.ENLIGHTENMENT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })
    return xi.effect.ENLIGHTENMENT
end

xi.job_utils.scholar.checkCaperEmissarius = function(player, target, ability)
    local msg, param = xi.job_utils.scholar.checkCaperEmissariusFromParams({
        hasTarget   = target ~= nil,
        isPC        = target ~= nil and target:isPC(),
        sameAsActor = target ~= nil and target:getID() == player:getID(),
    })
    if msg ~= 0 then
        return msg, param
    end

    ability:setRecast(xi.job_utils.scholar.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.scholar.useCaperEmissarius = function(player, target, ability)
    local percent, rangeYalms = xi.job_utils.scholar.caperTransferParamsFromParams()
    target:transferEnmity(player, percent, rangeYalms)
end
