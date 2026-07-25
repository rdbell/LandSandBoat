-----------------------------------
-- Global file for additional effects (Status Effects)
-- Pure injects dual-wired to OmegaXI internal/addeffectstatus (slice 6702 / 0943 / 6094).
-- Shares enspellEffects / hasEnspellFromParams / procMiss with damage path.
-----------------------------------
require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/combat/action_additional_effect_damage') -- enspell + procMiss pure
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.action = xi.combat.action or {}
-----------------------------------

xi.combat.action.addEffectStatusDefaultChance = 100
xi.combat.action.addEffectStatusDefaultDuration = 120

-- Effect → { subEffect animation, message } defaults.
xi.combat.action.addEffectStatusDefaultsTable =
{
    [xi.effect.AMNESIA      ] = { xi.subEffect.AMNESIA,         xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.ATTACK_DOWN  ] = { xi.subEffect.ATTACK_DOWN,     xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.BIND         ] = { xi.subEffect.DARKNESS_DAMAGE, xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.BLINDNESS    ] = { xi.subEffect.BLIND,           xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.CURSE_I      ] = { xi.subEffect.CURSE,           xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.DEFENSE_DOWN ] = { xi.subEffect.DEFENSE_DOWN,    xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.EVASION_DOWN ] = { xi.subEffect.EVASION_DOWN,    xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.KO           ] = { xi.subEffect.DEATH,           xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.NONE         ] = { xi.subEffect.DARKNESS_DAMAGE, xi.msg.basic.ADD_EFFECT_DISPEL },
    [xi.effect.PARALYSIS    ] = { xi.subEffect.PARALYSIS,       xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.PETRIFICATION] = { xi.subEffect.PETRIFY,         xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.PLAGUE       ] = { xi.subEffect.PLAGUE,          xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.POISON       ] = { xi.subEffect.POISON,          xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.SILENCE      ] = { xi.subEffect.SILENCE,         xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.SLEEP_I      ] = { xi.subEffect.SLEEP,           xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.SLOW         ] = { xi.subEffect.SLOW,            xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.STUN         ] = { xi.subEffect.STUN,            xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.TERROR       ] = { xi.subEffect.PARALYSIS,       xi.msg.basic.ADD_EFFECT_STATUS },
    [xi.effect.WEIGHT       ] = { xi.subEffect.ATTACK_DOWN,     xi.msg.basic.ADD_EFFECT_STATUS },
}

-----------------------------------
-- Pure injects
-----------------------------------

xi.combat.action.defaultsForStatusEffect = function(effectId)
    local row = xi.combat.action.addEffectStatusDefaultsTable[effectId or xi.effect.NONE]
    if not row then
        return 0, 0
    end

    return row[1] or 0, row[2] or 0
end

-- Pure validateParameters once fedData is injected (aeTarget deferred to host).
xi.combat.action.validateAddEffectStatusParams = function(fedData)
    fedData = fedData or {}
    local params = {}

    params.chance       = fedData.chance or xi.combat.action.addEffectStatusDefaultChance
    params.effectId     = fedData.effectId or xi.effect.NONE
    params.power        = fedData.power or 0
    params.tick         = fedData.tick or 0
    params.duration     = fedData.duration or xi.combat.action.addEffectStatusDefaultDuration
    params.subType      = fedData.subType or 0
    params.subPower     = fedData.subPower or 0
    params.tier         = fedData.tier or 0
    params.element      = fedData.element or xi.element.NONE
    params.actorStat    = fedData.actorStat or 0
    params.targetStat   = fedData.targetStat or params.actorStat
    params.macc         = fedData.macc or 0
    params.resistRate   = fedData.resistRate or 0
    params.resetEmnity  = fedData.resetEmnity or false
    params.absorbEffect = fedData.absorbEffect or false

    local defAnim, defMsg = xi.combat.action.defaultsForStatusEffect(params.effectId)
    params.animation    = fedData.animation or defAnim
    params.message      = fedData.message or defMsg

    return params
end

-- Pure gate compositions (slice 6094).
xi.combat.action.enhancementAppliesFromParams = function(params)
    if
        params.hasEnspell or
        (params.effectId or xi.effect.NONE) == xi.effect.NONE or
        params.procMiss or
        params.nullified
    then
        return false
    end

    return params.addStatusOK == true
end

xi.combat.action.enfeeblementAppliesFromParams = function(params)
    if
        params.hasEnspell or
        (params.effectId or xi.effect.NONE) == xi.effect.NONE or
        params.procMiss
    then
        return false
    end

    if
        params.immune or
        params.traitResisted or
        params.nullified or
        params.resistRateFail
    then
        return false
    end

    return params.addStatusOK == true
end

xi.combat.action.dispelAppliesFromParams = function(params)
    if params.hasEnspell then
        return false
    end

    if (params.effectId or xi.effect.NONE) ~= xi.effect.NONE then
        return false
    end

    if params.procMiss or not params.hasDispelable or params.resistRateFail then
        return false
    end

    return true
end

xi.combat.action.enfeebleDurationFromParams = function(baseDuration, resistanceRate)
    return math.floor((baseDuration or 0) * (resistanceRate or 0))
end

-----------------------------------
-- Entity hosts
-----------------------------------
xi.combat.action.executeAddEffectEnhancement = function(actor, target, fedData)
    local params = xi.combat.action.validateAddEffectStatusParams(fedData)
    params.aeTarget = (fedData and fedData.aeTarget) or target

    local hasEnspell = xi.combat.action.hasEnspellFromParams(function(id)
        return actor:hasStatusEffect(id)
    end)
    local procMiss = xi.combat.action.procMiss(math.random(1, 100), params.chance)
    local nullified = xi.data.statusEffect.isEffectNullified(params.aeTarget, params.effectId, params.tier)

    -- Gate composition without addStatus yet (early outs).
    if
        hasEnspell or
        params.effectId == xi.effect.NONE or
        procMiss or
        nullified
    then
        return 0, 0, 0
    end

    local addOK = params.aeTarget:addStatusEffect(params.effectId, {
        power = params.power, duration = params.duration, origin = actor,
        tick = params.tick, subType = params.subType, subPower = params.subPower, tier = params.tier,
    })

    if xi.combat.action.enhancementAppliesFromParams({
        hasEnspell = false, effectId = params.effectId, procMiss = false,
        nullified = false, addStatusOK = addOK,
    }) then
        return params.animation, params.message, params.effectId
    end

    return 0, 0, 0
end

xi.combat.action.executeAddEffectEnfeeblement = function(actor, target, fedData)
    local params = xi.combat.action.validateAddEffectStatusParams(fedData)
    params.aeTarget = (fedData and fedData.aeTarget) or target

    local hasEnspell = xi.combat.action.hasEnspellFromParams(function(id)
        return actor:hasStatusEffect(id)
    end)

    if hasEnspell or params.effectId == xi.effect.NONE then
        return 0, 0, 0
    end

    if xi.combat.action.procMiss(math.random(1, 100), params.chance) then
        return 0, 0, 0
    end

    if xi.data.statusEffect.isTargetImmune(params.aeTarget, params.effectId, params.element) then
        return 0, 0, 0
    end

    if xi.data.statusEffect.isTargetResistant(actor, params.aeTarget, params.effectId) then
        return 0, 0, 0
    end

    if xi.data.statusEffect.isEffectNullified(params.aeTarget, params.effectId, params.tier) then
        return 0, 0, 0
    end

    local resistanceRate = xi.combat.magicHitRate.calculateResistRate(
        actor, params.aeTarget, 0, 0, xi.skillRank.A_PLUS, params.element,
        params.actorStat, params.effectId, params.macc
    )
    if not xi.data.statusEffect.isResistRateSuccessfull(params.effectId, resistanceRate, params.resistRate) then
        return 0, 0, 0
    end

    local totalDuration = xi.combat.action.enfeebleDurationFromParams(params.duration, resistanceRate)

    if params.aeTarget:addStatusEffect(params.effectId, {
        power = params.power, duration = totalDuration, origin = actor,
        tick = params.tick, subType = params.subType, subPower = params.subPower, tier = params.tier,
    }) then
        return params.animation, params.message, params.effectId
    end

    return 0, 0, 0
end

xi.combat.action.executeAddEffectDispel = function(actor, target, fedData)
    local params = xi.combat.action.validateAddEffectStatusParams(fedData)
    params.aeTarget = (fedData and fedData.aeTarget) or target

    local hasEnspell = xi.combat.action.hasEnspellFromParams(function(id)
        return actor:hasStatusEffect(id)
    end)
    local procMiss = xi.combat.action.procMiss(math.random(1, 100), params.chance)
    local hasDispelable = params.aeTarget:hasStatusEffectByFlag(xi.effectFlag.DISPELABLE)

    local resistanceRate = 1
    local resistRateFail = false
    if
        not hasEnspell and
        params.effectId == xi.effect.NONE and
        not procMiss and
        hasDispelable
    then
        resistanceRate = xi.combat.magicHitRate.calculateResistRate(
            actor, params.aeTarget, 0, 0, xi.skillRank.A_PLUS, params.element,
            params.actorStat, params.effectId, params.macc
        )
        resistRateFail = not xi.data.statusEffect.isResistRateSuccessfull(
            params.effectId, resistanceRate, params.resistRate
        )
    end

    if not xi.combat.action.dispelAppliesFromParams({
        hasEnspell     = hasEnspell,
        effectId       = params.effectId,
        procMiss       = procMiss,
        hasDispelable  = hasDispelable,
        resistRateFail = resistRateFail,
    }) then
        return 0, 0, 0
    end

    local dispelledEffect = 0
    if params.absorbEffect then
        dispelledEffect = actor:stealStatusEffect(params.aeTarget, xi.effectFlag.DISPELABLE, true)
    else
        dispelledEffect = params.aeTarget:dispelStatusEffect(xi.effectFlag.DISPELABLE)
    end

    if dispelledEffect == 0 then
        return 0, 0, 0
    end

    return params.animation, params.message, dispelledEffect
end
