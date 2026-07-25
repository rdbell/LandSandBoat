-----------------------------------
-- Bard Job Utilities
-- Dual-wired pure inject forms (slice 6741 / 0898):
--   one-hour recast, Soul Voice/Pianissimo/Nightingale/Troubadour/
--   Tenuto/Marcato/Clarion Call fixed params
-- Parity: internal/bard
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.bard = xi.job_utils.bard or {}

-----------------------------------
-- Pure inject pins (internal/bard)
-----------------------------------
xi.job_utils.bard.soulVoicePower              = 1
xi.job_utils.bard.soulVoiceDuration           = 180
xi.job_utils.bard.pianissimoPower             = 0
xi.job_utils.bard.pianissimoDuration          = 60
xi.job_utils.bard.nightingalePower            = 0
xi.job_utils.bard.nightingaleDuration         = 60
xi.job_utils.bard.troubadourPower             = 0
xi.job_utils.bard.troubadourDuration          = 60
xi.job_utils.bard.tenutoPower                 = 0
xi.job_utils.bard.tenutoDuration              = 60
xi.job_utils.bard.marcatoPower                = 50
xi.job_utils.bard.marcatoDuration             = 60
xi.job_utils.bard.clarionCallPower            = 10
xi.job_utils.bard.clarionCallDuration         = 180
xi.job_utils.bard.oneHourRecastSecondsPerMod  = 60

-- Pure: OneHourRecast
xi.job_utils.bard.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.bard.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

xi.job_utils.bard.soulVoiceFromParams = function()
    return {
        power    = xi.job_utils.bard.soulVoicePower,
        duration = xi.job_utils.bard.soulVoiceDuration,
    }
end

xi.job_utils.bard.pianissimoFromParams = function()
    return {
        power    = xi.job_utils.bard.pianissimoPower,
        duration = xi.job_utils.bard.pianissimoDuration,
    }
end

xi.job_utils.bard.nightingaleFromParams = function()
    return {
        power    = xi.job_utils.bard.nightingalePower,
        duration = xi.job_utils.bard.nightingaleDuration,
    }
end

xi.job_utils.bard.troubadourFromParams = function()
    return {
        power    = xi.job_utils.bard.troubadourPower,
        duration = xi.job_utils.bard.troubadourDuration,
    }
end

xi.job_utils.bard.tenutoFromParams = function()
    return {
        power    = xi.job_utils.bard.tenutoPower,
        duration = xi.job_utils.bard.tenutoDuration,
    }
end

xi.job_utils.bard.marcatoFromParams = function()
    return {
        power    = xi.job_utils.bard.marcatoPower,
        duration = xi.job_utils.bard.marcatoDuration,
    }
end

xi.job_utils.bard.clarionCallFromParams = function()
    return {
        power    = xi.job_utils.bard.clarionCallPower,
        duration = xi.job_utils.bard.clarionCallDuration,
    }
end

-----------------------------------
-- Ability Check Functions
-----------------------------------
xi.job_utils.bard.checkSoulVoice = function(player, target, ability)
    ability:setRecast(xi.job_utils.bard.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.bard.checkClarionCall = function(player, target, ability)
    ability:setRecast(xi.job_utils.bard.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------
xi.job_utils.bard.useSoulVoice = function(player, target, ability)
    local p = xi.job_utils.bard.soulVoiceFromParams()
    player:addStatusEffect(xi.effect.SOUL_VOICE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.SOUL_VOICE
end

xi.job_utils.bard.usePianissimo = function(player, target, ability)
    local p = xi.job_utils.bard.pianissimoFromParams()
    player:addStatusEffect(xi.effect.PIANISSIMO, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.PIANISSIMO
end

xi.job_utils.bard.useNightingale = function(player, target, ability)
    local p = xi.job_utils.bard.nightingaleFromParams()
    player:addStatusEffect(xi.effect.NIGHTINGALE, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.NIGHTINGALE
end

xi.job_utils.bard.useTroubadour = function(player, target, ability)
    local p = xi.job_utils.bard.troubadourFromParams()
    player:addStatusEffect(xi.effect.TROUBADOUR, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.TROUBADOUR
end

xi.job_utils.bard.useTenuto = function(player, target, ability)
    -- TODO: Implement this ability
    local p = xi.job_utils.bard.tenutoFromParams()
    player:addStatusEffect(xi.effect.TENUTO, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.TENUTO
end

xi.job_utils.bard.useMarcato = function(player, target, ability)
    local p = xi.job_utils.bard.marcatoFromParams()
    player:addStatusEffect(xi.effect.MARCATO, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.MARCATO
end

xi.job_utils.bard.useClarionCall = function(player, target, ability)
    local p = xi.job_utils.bard.clarionCallFromParams()
    player:addStatusEffect(xi.effect.CLARION_CALL, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.CLARION_CALL
end
