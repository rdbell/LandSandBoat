-----------------------------------
-- Warrior Job Utilities
-- Dual-wired pure inject forms (slice 6730 / 0888):
--   levelScaleContribution, berserkPower/Duration, defenderPower/Duration,
--   aggressorPower/Duration, bloodRagePower/Duration, warcryPower/Duration,
--   tomahawkDuration, warriorsChargePower, oneHourRecast, checkTomahawkFromParams,
--   partyTargetMessage, activeJobLevel
-- Parity: internal/warrior
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.warrior = xi.job_utils.warrior or {}

-----------------------------------
-- Pure inject pins (internal/warrior)
-----------------------------------
xi.job_utils.warrior.berserkBasePower          = 25
xi.job_utils.warrior.defenderBasePower         = 25
xi.job_utils.warrior.bloodRageBasePower        = 20
xi.job_utils.warrior.bloodRageBaseDuration     = 30
xi.job_utils.warrior.standardStanceDuration    = 180
xi.job_utils.warrior.warcryBaseDuration        = 30
xi.job_utils.warrior.tomahawkBaseDuration      = 30
xi.job_utils.warrior.tomahawkMeritOffset       = 15
xi.job_utils.warrior.warriorsChargeMeritOffset = 5
xi.job_utils.warrior.warriorsChargeDuration    = 60
xi.job_utils.warrior.levelScaleFloor           = 40
xi.job_utils.warrior.levelScaleCap             = 10
xi.job_utils.warrior.oneHourRecastSecondsPerMod = 60
xi.job_utils.warrior.tomahawkPower             = 25
xi.job_utils.warrior.itemThrowingTomahawk      = 18258
xi.job_utils.warrior.msgCannotPerform          = 71

-- Pure main-WAR level scale (internal/warrior.LevelScaleContribution).
xi.job_utils.warrior.levelScaleContribution = function(mainJobIsWAR, mainLevel)
    local warriorLevel = 0
    if mainJobIsWAR then
        warriorLevel = mainLevel or 0
    end

    local levelScale = math.floor((warriorLevel - xi.job_utils.warrior.levelScaleFloor) / 10) * 2
    return utils.clamp(levelScale, 0, xi.job_utils.warrior.levelScaleCap)
end

-- Pure Berserk power/duration.
xi.job_utils.warrior.berserkPowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.berserkBasePower
        + (params.berserkPotencyMod or 0)
        + xi.job_utils.warrior.levelScaleContribution(params.mainJobIsWAR, params.mainLevel or 0)
end

xi.job_utils.warrior.berserkDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.standardStanceDuration + (params.berserkDurationMod or 0)
end

-- Pure Defender power/duration.
xi.job_utils.warrior.defenderPowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.defenderBasePower
        + xi.job_utils.warrior.levelScaleContribution(params.mainJobIsWAR, params.mainLevel or 0)
end

xi.job_utils.warrior.defenderDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.standardStanceDuration + (params.defenderDurationMod or 0)
end

-- Pure Aggressor power/duration.
xi.job_utils.warrior.aggressorPowerFromParams = function(params)
    params = params or {}
    return params.aggressiveAimMerit or 0
end

xi.job_utils.warrior.aggressorDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.standardStanceDuration + (params.aggressorDurationMod or 0)
end

-- Pure Blood Rage power/duration.
xi.job_utils.warrior.bloodRagePowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.bloodRageBasePower + (params.bloodRageJP or 0)
end

xi.job_utils.warrior.bloodRageDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.bloodRageBaseDuration + (params.enhancesBloodRageMod or 0)
end

-- Pure Warcry power/duration/subPower.
xi.job_utils.warrior.warcryPowerFromParams = function(params)
    params = params or {}
    local warLevel = params.warLevel or 0
    return (math.floor((warLevel / 4) + 4.75) / 256) * 100
end

xi.job_utils.warrior.warcryDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.warcryBaseDuration + (params.warcryDurationMod or 0)
end

xi.job_utils.warrior.warcrySubPowerFromParams = function(params)
    params = params or {}
    return params.savageryMerit or 0
end

-- Pure Tomahawk duration / Warriors Charge power.
xi.job_utils.warrior.tomahawkDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.warrior.tomahawkBaseDuration
        + ((params.tomahawkMerit or 0) - xi.job_utils.warrior.tomahawkMeritOffset)
end

xi.job_utils.warrior.warriorsChargePowerFromParams = function(params)
    params = params or {}
    return (params.warriorsChargeMerit or 0) - xi.job_utils.warrior.warriorsChargeMeritOffset
end

-- Pure one-hour recast reduction.
xi.job_utils.warrior.oneHourRecastFromParams = function(params)
    params = params or {}
    return math.max(0, (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.warrior.oneHourRecastSecondsPerMod)
end

-- Pure Tomahawk ammo gate (returns msg id; 0 = OK).
xi.job_utils.warrior.checkTomahawkFromParams = function(params)
    params = params or {}
    if (params.ammoID or 0) == xi.job_utils.warrior.itemThrowingTomahawk then
        return 0
    end

    return xi.job_utils.warrior.msgCannotPerform
end

-- Pure party-target setMsg gate (Blood Rage / Warcry).
-- returns: msg, changed
xi.job_utils.warrior.partyTargetMessage = function(actorID, targetID, otherMsg)
    if actorID ~= targetID then
        return otherMsg, true
    end

    return 0, false
end

-- Pure utils.getActiveJobLevel inject.
xi.job_utils.warrior.activeJobLevel = function(mainJob, subJob, job, mainLevel, subLevel)
    if mainJob == job then
        return mainLevel or 0
    end

    if subJob == job then
        return subLevel or 0
    end

    return 0
end

-----------------------------------
-- Ability Check Functions (hosts → pure)
-----------------------------------
xi.job_utils.warrior.checkBrazenRush = function(player, target, ability)
    ability:setRecast(xi.job_utils.warrior.oneHourRecastFromParams({
        abilityRecast     = ability:getRecast(),
        oneHourRecastMod  = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.warrior.checkMightyStrikes = function(player, target, ability)
    ability:setRecast(xi.job_utils.warrior.oneHourRecastFromParams({
        abilityRecast     = ability:getRecast(),
        oneHourRecastMod  = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.warrior.checkTomahawk = function(player, target, ability)
    local msg = xi.job_utils.warrior.checkTomahawkFromParams({
        ammoID = player:getEquipID(xi.slot.AMMO),
    })
    return msg, 0
end

-----------------------------------
-- Ability Use Functions (hosts → pure)
-----------------------------------
xi.job_utils.warrior.useAggressor = function(player, target, ability)
    local power = xi.job_utils.warrior.aggressorPowerFromParams({
        aggressiveAimMerit = player:getMerit(xi.merit.AGGRESSIVE_AIM),
    })
    local duration = xi.job_utils.warrior.aggressorDurationFromParams({
        aggressorDurationMod = player:getMod(xi.mod.AGGRESSOR_DURATION),
    })

    player:addStatusEffect(xi.effect.AGGRESSOR, { power = power, duration = duration, origin = player })

    return xi.effect.AGGRESSOR
end

xi.job_utils.warrior.useBerserk = function(player, target, ability)
    local power = xi.job_utils.warrior.berserkPowerFromParams({
        mainJobIsWAR      = player:getMainJob() == xi.job.WAR,
        mainLevel         = player:getMainLvl(),
        berserkPotencyMod = player:getMod(xi.mod.BERSERK_POTENCY),
    })
    local duration = xi.job_utils.warrior.berserkDurationFromParams({
        berserkDurationMod = player:getMod(xi.mod.BERSERK_DURATION),
    })

    player:addStatusEffect(xi.effect.BERSERK, { power = power, duration = duration, origin = player })

    return xi.effect.BERSERK
end

xi.job_utils.warrior.useBloodRage = function(player, target, ability)
    local power = xi.job_utils.warrior.bloodRagePowerFromParams({
        bloodRageJP = player:getJobPointLevel(xi.jp.BLOOD_RAGE_EFFECT),
    })
    local duration = xi.job_utils.warrior.bloodRageDurationFromParams({
        enhancesBloodRageMod = player:getMod(xi.mod.ENHANCES_BLOOD_RAGE),
    })

    target:addStatusEffect(xi.effect.BLOOD_RAGE, { power = power, duration = duration, origin = player })

    local msg, changed = xi.job_utils.warrior.partyTargetMessage(
        player:getID(), target:getID(), xi.msg.basic.JA_GAIN_EFFECT)
    if changed then
        ability:setMsg(msg)
    end

    return xi.effect.BLOOD_RAGE
end

xi.job_utils.warrior.useBrazenRush = function(player, target, ability)
    player:addStatusEffect(xi.effect.BRAZEN_RUSH, { power = 100, duration = 30, origin = player, tick = 3 })

    return xi.effect.BRAZEN_RUSH
end

xi.job_utils.warrior.useDefender = function(player, target, ability)
    local power = xi.job_utils.warrior.defenderPowerFromParams({
        mainJobIsWAR = player:getMainJob() == xi.job.WAR,
        mainLevel    = player:getMainLvl(),
    })
    local duration = xi.job_utils.warrior.defenderDurationFromParams({
        defenderDurationMod = player:getMod(xi.mod.DEFENDER_DURATION),
    })

    player:addStatusEffect(xi.effect.DEFENDER, { power = power, duration = duration, origin = player })

    return xi.effect.DEFENDER
end

xi.job_utils.warrior.useMightyStrikes = function(player, target, ability)
    player:addStatusEffect(xi.effect.MIGHTY_STRIKES, { power = 1, duration = 45, origin = player })

    return xi.effect.MIGHTY_STRIKES
end

xi.job_utils.warrior.useRestraint = function(player, target, ability)
    player:addStatusEffect(xi.effect.RESTRAINT, { duration = 300, origin = player })

    return xi.effect.RESTRAINT
end

xi.job_utils.warrior.useRetaliation = function(player, target, ability)
    player:addStatusEffect(xi.effect.RETALIATION, { power = 1, duration = 180, origin = player })

    return xi.effect.RETALIATION
end

xi.job_utils.warrior.useTomahawk = function(player, target, ability)
    local duration = xi.job_utils.warrior.tomahawkDurationFromParams({
        tomahawkMerit = player:getMerit(xi.merit.TOMAHAWK),
    })

    target:addStatusEffect(xi.effect.TOMAHAWK, {
        power    = xi.job_utils.warrior.tomahawkPower,
        duration = duration,
        origin   = player,
        tick     = 3,
        icon     = 0,
    })
    player:removeAmmo(1)
end

xi.job_utils.warrior.useWarcry = function(player, target, ability)
    local warLevel = utils.getActiveJobLevel(player, xi.job.WAR)
    local power = xi.job_utils.warrior.warcryPowerFromParams({ warLevel = warLevel })
    local duration = xi.job_utils.warrior.warcryDurationFromParams({
        warcryDurationMod = player:getMod(xi.mod.WARCRY_DURATION),
    })
    local subPower = xi.job_utils.warrior.warcrySubPowerFromParams({
        savageryMerit = player:getMerit(xi.merit.SAVAGERY),
    })

    target:addStatusEffect(xi.effect.WARCRY, {
        power    = power,
        duration = duration,
        origin   = player,
        subPower = subPower,
    })

    local msg, changed = xi.job_utils.warrior.partyTargetMessage(
        player:getID(), target:getID(), xi.msg.basic.JA_ATK_ENHANCED)
    if changed then
        ability:setMsg(msg)
    end

    return xi.effect.WARCRY
end

xi.job_utils.warrior.useWarriorsCharge = function(player, target, ability, action)
    local power = xi.job_utils.warrior.warriorsChargePowerFromParams({
        warriorsChargeMerit = player:getMerit(xi.merit.WARRIORS_CHARGE),
    })

    player:addStatusEffect(xi.effect.WARRIORS_CHARGE, {
        power    = power,
        duration = xi.job_utils.warrior.warriorsChargeDuration,
        origin   = player,
    })

    return xi.effect.WARRIORS_CHARGE
end
