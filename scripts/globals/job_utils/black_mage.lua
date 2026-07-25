-----------------------------------
-- Black Mage Job Utilities
-- Dual-wired pure inject forms (slice 6738 / 0896):
--   one-hour recast, Cascade/Elemental Seal/Manafont/Mana Wall/
--   Manawell/Subtle Sorcery fixed params, Enmity Douse CE/VE pins
-- Parity: internal/blackmage
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.black_mage = xi.job_utils.black_mage or {}

-----------------------------------
-- Pure inject pins (internal/blackmage)
-----------------------------------
xi.job_utils.black_mage.cascadePower               = 1
xi.job_utils.black_mage.cascadeDuration            = 60
xi.job_utils.black_mage.elementalSealPower         = 1
xi.job_utils.black_mage.elementalSealDuration      = 60
xi.job_utils.black_mage.manafontPower              = 1
xi.job_utils.black_mage.manafontDuration           = 60
xi.job_utils.black_mage.manaWallPower              = 1
xi.job_utils.black_mage.manaWallDuration           = 300
xi.job_utils.black_mage.manawellPower              = 1
xi.job_utils.black_mage.manawellDuration           = 60
xi.job_utils.black_mage.subtleSorceryPower         = 1
xi.job_utils.black_mage.subtleSorceryDuration      = 60
xi.job_utils.black_mage.enmityDouseCE              = 1
xi.job_utils.black_mage.enmityDouseVE              = 0
xi.job_utils.black_mage.oneHourRecastSecondsPerMod = 60

-- Pure: OneHourRecast
xi.job_utils.black_mage.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.black_mage.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

-- Pure fixed effect params
xi.job_utils.black_mage.cascadeFromParams = function()
    return {
        power    = xi.job_utils.black_mage.cascadePower,
        duration = xi.job_utils.black_mage.cascadeDuration,
    }
end

xi.job_utils.black_mage.elementalSealFromParams = function()
    return {
        power    = xi.job_utils.black_mage.elementalSealPower,
        duration = xi.job_utils.black_mage.elementalSealDuration,
    }
end

xi.job_utils.black_mage.manafontFromParams = function()
    return {
        power    = xi.job_utils.black_mage.manafontPower,
        duration = xi.job_utils.black_mage.manafontDuration,
    }
end

xi.job_utils.black_mage.manaWallFromParams = function()
    return {
        power    = xi.job_utils.black_mage.manaWallPower,
        duration = xi.job_utils.black_mage.manaWallDuration,
    }
end

xi.job_utils.black_mage.manawellFromParams = function()
    return {
        power    = xi.job_utils.black_mage.manawellPower,
        duration = xi.job_utils.black_mage.manawellDuration,
    }
end

xi.job_utils.black_mage.subtleSorceryFromParams = function()
    return {
        power    = xi.job_utils.black_mage.subtleSorceryPower,
        duration = xi.job_utils.black_mage.subtleSorceryDuration,
    }
end

-- Pure: Enmity Douse CE/VE when isMob
-- params: isMob
-- returns: applies, ce, ve
xi.job_utils.black_mage.enmityDouseFromParams = function(params)
    params = params or {}
    if not params.isMob then
        return false, 0, 0
    end

    return true, xi.job_utils.black_mage.enmityDouseCE, xi.job_utils.black_mage.enmityDouseVE
end

-----------------------------------
-- Ability Check Functions
-----------------------------------
xi.job_utils.black_mage.checkManafont = function(player, target, ability)
    ability:setRecast(xi.job_utils.black_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.black_mage.checkSubtleSorcery = function(player, target, ability)
    ability:setRecast(xi.job_utils.black_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------
xi.job_utils.black_mage.useCascade = function(player, target, ability)
    local p = xi.job_utils.black_mage.cascadeFromParams()
    player:addStatusEffect(xi.effect.CASCADE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.CASCADE
end

xi.job_utils.black_mage.useElementalSeal = function(player, target, ability)
    local p = xi.job_utils.black_mage.elementalSealFromParams()
    player:addStatusEffect(xi.effect.ELEMENTAL_SEAL, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.ELEMENTAL_SEAL
end

xi.job_utils.black_mage.useEnmityDouse = function(player, target, ability)
    local applies, ce, ve = xi.job_utils.black_mage.enmityDouseFromParams({
        isMob = target:isMob(),
    })
    if applies then
        target:setCE(player, ce)
        target:setVE(player, ve)
    end
end

xi.job_utils.black_mage.useManafont = function(player, target, ability)
    local p = xi.job_utils.black_mage.manafontFromParams()
    player:addStatusEffect(xi.effect.MANAFONT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.MANAFONT
end

xi.job_utils.black_mage.useManaWall = function(player, target, ability)
    local p = xi.job_utils.black_mage.manaWallFromParams()
    player:addStatusEffect(xi.effect.MANA_WALL, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.MANA_WALL
end

xi.job_utils.black_mage.useManawell = function(player, target, ability)
    local p = xi.job_utils.black_mage.manawellFromParams()
    target:addStatusEffect(xi.effect.MANAWELL, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.MANAWELL
end

xi.job_utils.black_mage.useSubtleSorcery = function(player, target, ability)
    local p = xi.job_utils.black_mage.subtleSorceryFromParams()
    player:addStatusEffect(xi.effect.SUBTLE_SORCERY, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.SUBTLE_SORCERY
end
