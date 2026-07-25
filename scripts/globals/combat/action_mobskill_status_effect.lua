-----------------------------------
-- Global file for mobskills that apply status effects.
-- Pure injects dual-wired to OmegaXI internal/mobskilleffect (slice 6703 / 0939 / 6106).
-----------------------------------
require('scripts/globals/combat/damage_multipliers')
require('scripts/globals/combat/magic_hit_rate')
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.action = xi.combat.action or {}
-----------------------------------

-- Define each step where the process can end.
xi.combat.action.mobskillStatusStep =
{
    CANT_GAIN           = 1,
    IMMUNE_CHECK        = 2,
    RESIST_TRAIT_CHECK  = 3,
    NULLIFY_CHECK       = 4,
    RESIST_RATE_CHECK   = 5,
    APPLICATION_FAIL    = 6,
    APPLICATION_SUCCESS = 7,
}

xi.combat.action.mobskillStatusDefaultRank = xi.skillRank.A_PLUS
xi.combat.action.mobskillStatusDefaultStat = xi.mod.INT

-----------------------------------
-- Pure injects
-----------------------------------

-- Pure validateEffectParameters once fedData and associated element are injected.
-- associatedElement is used only when fedData.element is nil.
xi.combat.action.validateMobskillEffectParams = function(fedData, associatedElement)
    fedData = fedData or {}
    local params = {}

    params.effectId   = fedData.effectId or 0
    params.power      = fedData.power or 0
    params.tick       = fedData.tick or 0
    params.duration   = fedData.duration or 0
    params.subType    = fedData.subType or 0
    params.subPower   = fedData.subPower or 0
    params.tier       = fedData.tier or 0

    if fedData.element ~= nil then
        params.element = fedData.element
    else
        params.element = associatedElement or xi.element.NONE
    end

    if fedData.rank ~= nil then
        params.rank = fedData.rank
    else
        params.rank = xi.combat.action.mobskillStatusDefaultRank
    end

    if fedData.stat ~= nil then
        params.stat = fedData.stat
    else
        params.stat = xi.combat.action.mobskillStatusDefaultStat
    end

    params.macc       = fedData.macc or 0
    params.resistRate = fedData.resistRate or 0

    return params
end

-- Pure validateMessageParameters.
xi.combat.action.validateMobskillMessageParams = function(fedData)
    fedData = fedData or {}
    local params = {}

    params.messageBypass          = fedData.messageBypass or false
    params.messageCantGain        = fedData.messageCantGain or xi.msg.basic.SKILL_NO_EFFECT
    params.messageIsImmune        = fedData.messageIsImmune or xi.msg.basic.SKILL_MISS
    params.messageIsTraitResisted = fedData.messageIsTraitResisted or xi.msg.basic.SKILL_MISS
    params.messageIsIncompatible  = fedData.messageIsIncompatible or xi.msg.basic.SKILL_MISS
    params.messageIsResisted      = fedData.messageIsResisted or xi.msg.basic.SKILL_MISS
    params.messageIsNotSuccessful = fedData.messageIsNotSuccessful or xi.msg.basic.SKILL_MISS
    params.messageIsSuccessful    = fedData.messageIsSuccessful or xi.msg.basic.SKILL_ENFEEB_IS

    return params
end

xi.combat.action.durationAfterResist = function(duration, resistanceRate)
    return math.floor((duration or 0) * (resistanceRate or 0))
end

-- Pure handleStatusEffect gate ladder once entity checks are injected as bools.
-- params: canGain, isImmune, isTraitResisted, isNullified, resistSuccess, addSucceeded
xi.combat.action.selectMobskillStatusStep = function(params)
    local step = xi.combat.action.mobskillStatusStep

    if not params.canGain then
        return step.CANT_GAIN
    end

    if params.isImmune then
        return step.IMMUNE_CHECK
    end

    if params.isTraitResisted then
        return step.RESIST_TRAIT_CHECK
    end

    if params.isNullified then
        return step.NULLIFY_CHECK
    end

    if not params.resistSuccess then
        return step.RESIST_RATE_CHECK
    end

    if params.addSucceeded then
        return step.APPLICATION_SUCCESS
    end

    return step.APPLICATION_FAIL
end

-- Pure message for best step. Returns messageId, set (false when bypass/unknown).
xi.combat.action.messageForMobskillStatusStep = function(bestResult, messageParams)
    if messageParams.messageBypass then
        return 0, false
    end

    local step = xi.combat.action.mobskillStatusStep
    if bestResult == step.CANT_GAIN then
        return messageParams.messageCantGain, true
    elseif bestResult == step.IMMUNE_CHECK then
        return messageParams.messageIsImmune, true
    elseif bestResult == step.RESIST_TRAIT_CHECK then
        return messageParams.messageIsTraitResisted, true
    elseif bestResult == step.NULLIFY_CHECK then
        return messageParams.messageIsIncompatible, true
    elseif bestResult == step.RESIST_RATE_CHECK then
        return messageParams.messageIsResisted, true
    elseif bestResult == step.APPLICATION_FAIL then
        return messageParams.messageIsNotSuccessful, true
    elseif bestResult == step.APPLICATION_SUCCESS then
        return messageParams.messageIsSuccessful, true
    end

    return 0, false
end

-- Pure best-outcome selection: highest step wins; ties keep earliest index.
-- results is { { effectId, step }, ... }
-- returns effectId, bestStep, bestIndex (1-based; 0 when empty)
xi.combat.action.selectBestMobskillStatusResult = function(results)
    if not results or #results == 0 then
        return 0, 0, 0
    end

    local bestResult = results[1][2]
    local bestIndex = 1

    for j = 2, #results do
        local currentResult = results[j][2]
        if currentResult > bestResult then
            bestResult = currentResult
            bestIndex = j
        end
    end

    return results[bestIndex][1], bestResult, bestIndex
end

-----------------------------------
-- Entity host helpers
-----------------------------------

local function handleStatusEffect(actor, target, params)
    local canGain = target:canGainStatusEffect(params.effectId, params.power)
    local isImmune = false
    local isTraitResisted = false
    local isNullified = false
    local resistSuccess = false
    local addSucceeded = false
    local resistanceRate = 1

    if canGain then
        isImmune = xi.data.statusEffect.isTargetImmune(target, params.effectId, params.element)
        if not isImmune then
            isTraitResisted = xi.data.statusEffect.isTargetResistant(actor, target, params.effectId)
            if not isTraitResisted then
                isNullified = xi.data.statusEffect.isEffectNullified(target, params.effectId, params.tier)
                if not isNullified then
                    resistanceRate = xi.combat.magicHitRate.calculateResistRate(
                        actor, target, 0, 0, params.rank, params.element,
                        params.stat, params.effectId, params.macc
                    )
                    resistSuccess = xi.data.statusEffect.isResistRateSuccessfull(
                        params.effectId, resistanceRate, params.resistRate
                    )
                    if resistSuccess then
                        local totalDuration = xi.combat.action.durationAfterResist(params.duration, resistanceRate)
                        addSucceeded = target:addStatusEffect(params.effectId, {
                            power = params.power, duration = totalDuration, origin = actor,
                            tick = params.tick, subType = params.subType, subPower = params.subPower, tier = params.tier,
                        })
                    end
                end
            end
        end
    end

    return xi.combat.action.selectMobskillStatusStep({
        canGain         = canGain,
        isImmune        = isImmune,
        isTraitResisted = isTraitResisted,
        isNullified     = isNullified,
        resistSuccess   = resistSuccess,
        addSucceeded    = addSucceeded,
    })
end

local function handleActionMessage(skill, bestResult, messageParams)
    local msg, set = xi.combat.action.messageForMobskillStatusStep(bestResult, messageParams)
    if set then
        skill:setMsg(msg)
    end
end

xi.combat.action.executeMobskillStatusEffect = function(actor, target, skill, effectData, messageData)
    -- Cycle over all effects. Apply (or not) and save the result in a table.
    local dTableEffectResults = {}
    for i = 1, #effectData do
        local associatedElement = xi.data.statusEffect.getAssociatedElement(
            effectData[i].effectId or 0, xi.element.NONE
        )
        local effectParams = xi.combat.action.validateMobskillEffectParams(effectData[i], associatedElement)
        local result       = handleStatusEffect(actor, target, effectParams)
        table.insert(dTableEffectResults, i, { effectParams.effectId, result })
    end

    local effectId, bestResult = xi.combat.action.selectBestMobskillStatusResult(dTableEffectResults)
    if bestResult == 0 then
        return 0
    end

    local messageParams = xi.combat.action.validateMobskillMessageParams(messageData)
    handleActionMessage(skill, bestResult, messageParams)

    return effectId
end
