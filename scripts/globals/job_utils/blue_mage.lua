-----------------------------------
-- Blue Mage Job Utilities
-- Dual-wired pure inject forms (slice 6742 / 0900):
--   one-hour recast, Diffusion/Convergence already-active checks,
--   Azure Lore/Burst/Chain Affinity/Diffusion/Convergence/Efflux/
--   Unbridled Wisdom/Learning fixed params
-- Parity: internal/bluemage
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.blue_mage = xi.job_utils.blue_mage or {}
-----------------------------------

-----------------------------------
-- Pure inject pins (internal/bluemage)
-----------------------------------
xi.job_utils.blue_mage.azureLorePower               = 1
xi.job_utils.blue_mage.azureLoreDuration            = 30
xi.job_utils.blue_mage.burstAffinityPower           = 1
xi.job_utils.blue_mage.burstAffinityDuration        = 30
xi.job_utils.blue_mage.chainAffinityPower           = 1
xi.job_utils.blue_mage.chainAffinityDuration        = 30
xi.job_utils.blue_mage.diffusionPower               = 1
xi.job_utils.blue_mage.diffusionDuration            = 60
xi.job_utils.blue_mage.convergencePower             = 1
xi.job_utils.blue_mage.convergenceDuration          = 60
xi.job_utils.blue_mage.effluxPower                  = 16
xi.job_utils.blue_mage.effluxDuration               = 60
xi.job_utils.blue_mage.effluxTick                   = 1
xi.job_utils.blue_mage.unbridledWisdomPower         = 16
xi.job_utils.blue_mage.unbridledWisdomDuration      = 30
xi.job_utils.blue_mage.unbridledWisdomTick          = 1
xi.job_utils.blue_mage.unbridledLearningPower       = 16
xi.job_utils.blue_mage.unbridledLearningDuration    = 60
xi.job_utils.blue_mage.unbridledLearningTick        = 1
xi.job_utils.blue_mage.oneHourRecastSecondsPerMod   = 60
xi.job_utils.blue_mage.msgEffectAlreadyActive       = 523

-- Pure: OneHourRecast
xi.job_utils.blue_mage.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.blue_mage.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

-- Pure: checkDiffusion / checkConvergence
-- returns: msg, ok
xi.job_utils.blue_mage.checkDiffusionFromParams = function(params)
    params = params or {}
    if params.hasEffect then
        return xi.job_utils.blue_mage.msgEffectAlreadyActive, false
    end

    return 0, true
end

xi.job_utils.blue_mage.checkConvergenceFromParams = function(params)
    params = params or {}
    if params.hasEffect then
        return xi.job_utils.blue_mage.msgEffectAlreadyActive, false
    end

    return 0, true
end

-- Pure fixed effect params
xi.job_utils.blue_mage.azureLoreFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.azureLorePower,
        duration = xi.job_utils.blue_mage.azureLoreDuration,
    }
end

xi.job_utils.blue_mage.burstAffinityFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.burstAffinityPower,
        duration = xi.job_utils.blue_mage.burstAffinityDuration,
    }
end

xi.job_utils.blue_mage.chainAffinityFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.chainAffinityPower,
        duration = xi.job_utils.blue_mage.chainAffinityDuration,
    }
end

xi.job_utils.blue_mage.diffusionFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.diffusionPower,
        duration = xi.job_utils.blue_mage.diffusionDuration,
    }
end

xi.job_utils.blue_mage.convergenceFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.convergencePower,
        duration = xi.job_utils.blue_mage.convergenceDuration,
    }
end

xi.job_utils.blue_mage.effluxFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.effluxPower,
        duration = xi.job_utils.blue_mage.effluxDuration,
        tick     = xi.job_utils.blue_mage.effluxTick,
    }
end

xi.job_utils.blue_mage.unbridledWisdomFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.unbridledWisdomPower,
        duration = xi.job_utils.blue_mage.unbridledWisdomDuration,
        tick     = xi.job_utils.blue_mage.unbridledWisdomTick,
    }
end

xi.job_utils.blue_mage.unbridledLearningFromParams = function()
    return {
        power    = xi.job_utils.blue_mage.unbridledLearningPower,
        duration = xi.job_utils.blue_mage.unbridledLearningDuration,
        tick     = xi.job_utils.blue_mage.unbridledLearningTick,
    }
end

-----------------------------------
-- Ability Check Functions
-----------------------------------

xi.job_utils.blue_mage.checkAzureLore = function(player, target, ability)
    ability:setRecast(xi.job_utils.blue_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.blue_mage.checkBurstAffinity = function(player, target, ability)
    return 0, 0
end

xi.job_utils.blue_mage.checkChainAffinity = function(player, target, ability)
    return 0, 0
end

xi.job_utils.blue_mage.checkDiffusion = function(player, target, ability)
    local msg, ok = xi.job_utils.blue_mage.checkDiffusionFromParams({
        hasEffect = player:hasStatusEffect(xi.effect.DIFFUSION),
    })
    if ok then
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.blue_mage.checkConvergence = function(player, target, ability)
    local msg, ok = xi.job_utils.blue_mage.checkConvergenceFromParams({
        hasEffect = player:hasStatusEffect(xi.effect.CONVERGENCE),
    })
    if ok then
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.blue_mage.checkEfflux = function(player, target, ability)
    return 0, 0
end

xi.job_utils.blue_mage.checkUnbridledWisdom = function(player, target, ability)
    ability:setRecast(xi.job_utils.blue_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.blue_mage.checkUnbridledLearning = function(player, target, ability)
    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------

xi.job_utils.blue_mage.useAzureLore = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.azureLoreFromParams()
    player:addStatusEffect(xi.effect.AZURE_LORE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.AZURE_LORE
end

xi.job_utils.blue_mage.useBurstAffinity = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.burstAffinityFromParams()
    player:addStatusEffect(xi.effect.BURST_AFFINITY, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })
    return xi.effect.BURST_AFFINITY
end

xi.job_utils.blue_mage.useChainAffinity = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.chainAffinityFromParams()
    player:addStatusEffect(xi.effect.CHAIN_AFFINITY, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })
    return xi.effect.CHAIN_AFFINITY
end

xi.job_utils.blue_mage.useDiffusion = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.diffusionFromParams()
    player:addStatusEffect(xi.effect.DIFFUSION, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })
    return xi.effect.DIFFUSION
end

xi.job_utils.blue_mage.useConvergence = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.convergenceFromParams()
    player:addStatusEffect(xi.effect.CONVERGENCE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })
    return xi.effect.CONVERGENCE
end

xi.job_utils.blue_mage.useEfflux = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.effluxFromParams()
    player:addStatusEffect(xi.effect.EFFLUX, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
    })

    return xi.effect.EFFLUX
end

xi.job_utils.blue_mage.useUnbridledWisdom = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.unbridledWisdomFromParams()
    target:addStatusEffect(xi.effect.UNBRIDLED_WISDOM, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
    })

    return xi.effect.UNBRIDLED_WISDOM
end

xi.job_utils.blue_mage.useUnbridledLearning = function(player, target, ability, action)
    local p = xi.job_utils.blue_mage.unbridledLearningFromParams()
    target:addStatusEffect(xi.effect.UNBRIDLED_LEARNING, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
    })

    return xi.effect.UNBRIDLED_LEARNING
end
