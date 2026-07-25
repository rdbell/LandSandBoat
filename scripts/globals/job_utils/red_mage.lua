-----------------------------------
-- Red Mage Job Utilities
-- Dual-wired pure inject forms (slice 6743 / 0897):
--   one-hour recast, Chainspell/Composure/Saboteur/Spontaneity/Stymie
--   fixed params, Convert HP/MP swap products
-- Parity: internal/redmage
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.red_mage = xi.job_utils.red_mage or {}

-----------------------------------
-- Pure inject pins (internal/redmage)
-----------------------------------
xi.job_utils.red_mage.chainspellPower             = 1
xi.job_utils.red_mage.chainspellDuration          = 60
xi.job_utils.red_mage.composurePower              = 1
xi.job_utils.red_mage.composureDuration           = 7200
xi.job_utils.red_mage.saboteurPower               = 1
xi.job_utils.red_mage.saboteurDuration            = 60
xi.job_utils.red_mage.spontaneityPower            = 1
xi.job_utils.red_mage.spontaneityDuration         = 60
xi.job_utils.red_mage.stymiePower                 = 1
xi.job_utils.red_mage.stymieDuration              = 60
xi.job_utils.red_mage.oneHourRecastSecondsPerMod  = 60

-- Pure: OneHourRecast
xi.job_utils.red_mage.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.red_mage.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

xi.job_utils.red_mage.chainspellFromParams = function()
    return {
        power    = xi.job_utils.red_mage.chainspellPower,
        duration = xi.job_utils.red_mage.chainspellDuration,
    }
end

xi.job_utils.red_mage.composureFromParams = function()
    return {
        power    = xi.job_utils.red_mage.composurePower,
        duration = xi.job_utils.red_mage.composureDuration,
    }
end

xi.job_utils.red_mage.saboteurFromParams = function()
    return {
        power    = xi.job_utils.red_mage.saboteurPower,
        duration = xi.job_utils.red_mage.saboteurDuration,
    }
end

xi.job_utils.red_mage.spontaneityFromParams = function()
    return {
        power    = xi.job_utils.red_mage.spontaneityPower,
        duration = xi.job_utils.red_mage.spontaneityDuration,
    }
end

xi.job_utils.red_mage.stymieFromParams = function()
    return {
        power    = xi.job_utils.red_mage.stymiePower,
        duration = xi.job_utils.red_mage.stymieDuration,
    }
end

-- Pure: Convert JP extra HP
xi.job_utils.red_mage.convertJPExtraHPFromParams = function(params)
    params = params or {}
    return math.floor((params.playerMaxHP or 0) * (params.convertJP or 0) / 100)
end

-- Pure: Convert Murgleis/AUGMENTS_CONVERT extra HP
xi.job_utils.red_mage.convertMurgleisExtraHPFromParams = function(params)
    params = params or {}
    if (params.augmentsConvert or 0) <= 0 then
        return 0
    end

    return math.floor((params.playerMaxHP or 0) * (params.augmentsConvert or 0) / 100)
end

-- Pure: CanConvert
xi.job_utils.red_mage.canConvertFromParams = function(playerMP)
    return (playerMP or 0) > 0
end

-- Pure: ConvertSwap
-- params: playerHP, playerMP, playerMaxHP, convertJP, augmentsConvert
-- returns: newHP, newMP, ok
xi.job_utils.red_mage.convertSwapFromParams = function(params)
    params = params or {}
    if not xi.job_utils.red_mage.canConvertFromParams(params.playerMP) then
        return 0, 0, false
    end

    local jpExtra = xi.job_utils.red_mage.convertJPExtraHPFromParams({
        playerMaxHP = params.playerMaxHP,
        convertJP   = params.convertJP,
    })
    local murgleisExtra = xi.job_utils.red_mage.convertMurgleisExtraHPFromParams({
        playerMaxHP      = params.playerMaxHP,
        augmentsConvert  = params.augmentsConvert,
    })
    local newHP = (params.playerMP or 0) + jpExtra + murgleisExtra
    local newMP = params.playerHP or 0
    return newHP, newMP, true
end

-----------------------------------
-- Ability Check Functions
-----------------------------------
xi.job_utils.red_mage.checkChainspell = function(player, target, ability)
    ability:setRecast(xi.job_utils.red_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.red_mage.checkStymie = function(player, target, ability)
    ability:setRecast(xi.job_utils.red_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------
xi.job_utils.red_mage.useChainspell = function(player, target, ability)
    local p = xi.job_utils.red_mage.chainspellFromParams()
    player:addStatusEffect(xi.effect.CHAINSPELL, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.CHAINSPELL
end

xi.job_utils.red_mage.useComposure = function(player, target, ability)
    local p = xi.job_utils.red_mage.composureFromParams()
    player:delStatusEffect(xi.effect.COMPOSURE)
    player:addStatusEffect(xi.effect.COMPOSURE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.COMPOSURE
end

xi.job_utils.red_mage.useConvert = function(player, target, ability)
    local newHP, newMP, ok = xi.job_utils.red_mage.convertSwapFromParams({
        playerHP         = player:getHP(),
        playerMP         = player:getMP(),
        playerMaxHP      = player:getMaxHP(),
        convertJP        = player:getJobPointLevel(xi.jp.CONVERT_EFFECT),
        augmentsConvert  = player:getMod(xi.mod.AUGMENTS_CONVERT),
    })
    if ok then
        player:setHP(newHP)
        player:setMP(newMP)
    end
end

xi.job_utils.red_mage.useSaboteur = function(player, target, ability)
    local p = xi.job_utils.red_mage.saboteurFromParams()
    player:addStatusEffect(xi.effect.SABOTEUR, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.SABOTEUR
end

xi.job_utils.red_mage.useSpontaneity = function(player, target, ability)
    local p = xi.job_utils.red_mage.spontaneityFromParams()
    target:addStatusEffect(xi.effect.SPONTANEITY, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.SPONTANEITY
end

xi.job_utils.red_mage.useStymie = function(player, target, ability)
    local p = xi.job_utils.red_mage.stymieFromParams()
    target:addStatusEffect(xi.effect.STYMIE, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.STYMIE
end
