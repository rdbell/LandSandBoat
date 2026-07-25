-----------------------------------
-- Global, independent functions for physical hit rate calculations.
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.physicalHitRate = xi.combat.physicalHitRate or {}
-----------------------------------

-----------------------------------
-- Pure formula helpers (OmegaXI slice 6681)
-- Dual-wired so Go internal/physhitrate and LSB tests share one surface.
-----------------------------------

xi.combat.physicalHitRate.capPet           = 0.99
xi.combat.physicalHitRate.capPCH2H         = 0.99
xi.combat.physicalHitRate.capPCMainhand1H  = 0.99
xi.combat.physicalHitRate.capPCOffhandOr2H = 0.95
xi.combat.physicalHitRate.capNonPC         = 0.95
xi.combat.physicalHitRate.meleeHitRateFloor  = 0.2
xi.combat.physicalHitRate.rangedHitRateFloor = 0.05
xi.combat.physicalHitRate.rangedHitRateCap   = 0.95
xi.combat.physicalHitRate.maxRangedDistance  = 25
xi.combat.physicalHitRate.flashReductionPerMs = 0.03
xi.combat.physicalHitRate.levelCorrectionAccPerLevel = 4
xi.combat.physicalHitRate.avatarDlvlMin = 0
xi.combat.physicalHitRate.avatarDlvlMax = 38
xi.combat.physicalHitRate.baseHitPercent = 75

-- Pure hit-rate cap once entity predicates are known.
-- https://www.bg-wiki.com/ffxi/Hit_Rate
xi.combat.physicalHitRate.hitRateCap = function(isPet, isPC, usingH2H, isTwoHanded, slotLeftOrHigher)
    if isPet then
        return xi.combat.physicalHitRate.capPet
    elseif isPC then
        if usingH2H then
            return xi.combat.physicalHitRate.capPCH2H
        elseif isTwoHanded or slotLeftOrHigher then
            return xi.combat.physicalHitRate.capPCOffhandOr2H
        end

        return xi.combat.physicalHitRate.capPCMainhand1H
    end

    return xi.combat.physicalHitRate.capNonPC
end

-- Pure Flash accuracy penalty once remaining duration (ms) is known.
-- https://github.com/LandSandBoat/server/discussions/6926
xi.combat.physicalHitRate.flashPenalty = function(timeRemainingMs)
    if not timeRemainingMs or timeRemainingMs <= 0 then
        return 0
    end

    return math.floor(timeRemainingMs * xi.combat.physicalHitRate.flashReductionPerMs)
end

-- Pure level-correction ACC branch of accuracyAndEvasionToHitRate.
xi.combat.physicalHitRate.levelCorrectedAccuracy = function(acc, atkLvl, defLvl, applyLevelCorrection, isPC, isAvatar)
    if not applyLevelCorrection then
        return acc
    end

    local dlvl = atkLvl - defLvl

    if isAvatar then
        dlvl = utils.clamp(dlvl, xi.combat.physicalHitRate.avatarDlvlMin, xi.combat.physicalHitRate.avatarDlvlMax)
    end

    -- Accuracy Bonus, doesn't apply to PCs
    if not isPC and atkLvl > defLvl then
        acc = acc + dlvl * xi.combat.physicalHitRate.levelCorrectionAccPerLevel

    -- Accuracy Penalty, only applies to PCs
    elseif isPC and atkLvl < defLvl then
        acc = acc + dlvl * xi.combat.physicalHitRate.levelCorrectionAccPerLevel
    end

    return acc
end

-- Pure ACC/EVA → hit-rate (after level correction): (75 + (acc-eva)/2) / 100
xi.combat.physicalHitRate.accuracyEvasionToHitRate = function(acc, eva)
    local hitdiff = (acc - eva) / 2

    return (xi.combat.physicalHitRate.baseHitPercent + hitdiff) / 100
end

xi.combat.physicalHitRate.clampMeleeHitRate = function(hitrate, cap)
    return utils.clamp(hitrate, xi.combat.physicalHitRate.meleeHitRateFloor, cap)
end

xi.combat.physicalHitRate.clampRangedHitRate = function(hitrate)
    return utils.clamp(hitrate, xi.combat.physicalHitRate.rangedHitRateFloor, xi.combat.physicalHitRate.rangedHitRateCap)
end

-----------------------------------
-- Entity hosts
-----------------------------------

xi.combat.physicalHitRate.checkAnticipated = function(attacker, defender)
    -- Early Return: Defender lacks Third Eye.
    if not defender:hasStatusEffect(xi.effect.THIRD_EYE) then
        return false
    end

    -- Calculate chance to retain "Third Eye".
    local thirdEyeRetentionChance = 0
    local canRetainThirdEye = not defender:isPC() or defender:isPC() and defender:isWeaponTwoHanded()

    if defender:hasStatusEffect(xi.effect.SEIGAN) and canRetainThirdEye then
        -- Duration left.
        local thirdEyeEffect = defender:getStatusEffect(xi.effect.THIRD_EYE)
        local timeInEffect   = thirdEyeEffect:getDuration() - thirdEyeEffect:getTimeRemaining()

        -- Retention
        local retentionLossPerMillisecond = 1 / 300 -- Retain 100% / 30 seconds (30000 milliseconds)
        local retentionModifier           = utils.clamp(1 - defender:getMod(xi.mod.THIRD_EYE_RETENTION_RATE) / 100, 0, 1) -- 50 = 0.5x reduction in loss per millisecond

        -- Add in retention bonus, Kogarasumaru has a 50% reduction per JP wiki -- https://wiki.ffo.jp/html/15175.html
        -- Other sources such as BG indicate it has a reduction -- https://www.bluegartr.com/threads/71538-Mythic-Weapon-Compiled-Information?p=2972086&highlight=#post2972086

        -- Increase scale by 100x to give more precision to the RNG
        thirdEyeRetentionChance = utils.clamp(100 - timeInEffect * retentionLossPerMillisecond * retentionModifier, 0, 100) * 100
    end

    -- Calculate if "Third Eye" is retained.
    if
        thirdEyeRetentionChance == 0 or
        math.random(1, 10000) > thirdEyeRetentionChance
    then
        defender:delStatusEffect(xi.effect.THIRD_EYE)
    end

    return true
end

-- https://www.bg-wiki.com/ffxi/Hit_Rate
-- This is only for melee attacks
---@param attacker CBaseEntity
---@param slot xi.attackAnimation
---@return number
xi.combat.physicalHitRate.getPhysicalHitRateCap = function(attacker, slot)
    return xi.combat.physicalHitRate.hitRateCap(
        attacker:isPet(),
        attacker:isPC(),
        attacker:isUsingH2H(), -- Kicks aren't explicitly listed as 99%, TODO: needs verification
        attacker:isWeaponTwoHanded(),
        slot >= xi.attackAnimation.LEFT_ATTACK -- 1h offhand, ranged
    )
end

---@param entity CBaseEntity
---@return number
xi.combat.physicalHitRate.getFlashPenalty = function(entity)
    local effect = entity:getStatusEffect(xi.effect.FLASH)

    if effect then
        return xi.combat.physicalHitRate.flashPenalty(effect:getTimeRemaining())
    end

    return 0
end

-----------------------------------
-- Pure hit-rate modifier injects (OmegaXI slice 6683)
-- Dual-wired to internal/hitratemod.HitRateModifiers.
-----------------------------------

xi.combat.physicalHitRate.behindAngle          = 23
xi.combat.physicalHitRate.facingConeYonin      = 64
xi.combat.physicalHitRate.buildingFlourishBase = 40
xi.combat.physicalHitRate.buildingFlourishSubPowerScale = 2
xi.combat.physicalHitRate.yoninJPScale         = 2

-- Pure getHitRateModifiers once status/merit/JP/geometry/Flash are injected.
-- params fields:
--   isRanged, isWeaponskill,
--   hasBuildingFlourish, buildingFlourishPower, buildingFlourishSubPower,
--   hasInnin, inninPower, inninJP, isBehind23,
--   attackerIsPC, attackerIsFacing, attackerClosedPositionMerit,
--   hasAmbushTrait, ambushMerit,
--   hasYonin, yoninPower, yoninJP, isFacing64,
--   targetIsPC, targetIsFacing, targetClosedPositionMerit,
--   flashPenalty
-- returns accBonus, evaBonus
xi.combat.physicalHitRate.hitRateModifiersFromParams = function(params)
    local accBonus = 0
    local evaBonus = 0
    local isRanged = params.isRanged or false

    -- Melee only: Building Flourish, Innin, attacker Closed Position.
    if not isRanged then
        if
            params.isWeaponskill and
            params.hasBuildingFlourish and
            (params.buildingFlourishPower or 0) >= 1
        then
            accBonus = xi.combat.physicalHitRate.buildingFlourishBase +
                (params.buildingFlourishSubPower or 0) * xi.combat.physicalHitRate.buildingFlourishSubPowerScale
        end

        if params.hasInnin and params.isBehind23 then
            accBonus = accBonus + (params.inninPower or 0) + (params.inninJP or 0)
        end

        if params.attackerIsPC and params.attackerIsFacing then
            accBonus = accBonus + (params.attackerClosedPositionMerit or 0)
        end
    end

    -- Ambush: melee and ranged.
    if params.hasAmbushTrait and params.isBehind23 then
        accBonus = accBonus + (params.ambushMerit or 0)
    end

    -- Yonin: melee and ranged.
    if params.hasYonin and params.isFacing64 then
        evaBonus = evaBonus + (params.yoninPower or 0) +
            xi.combat.physicalHitRate.yoninJPScale * (params.yoninJP or 0)
    end

    -- Target Closed Position.
    if params.targetIsPC and params.targetIsFacing then
        evaBonus = evaBonus + (params.targetClosedPositionMerit or 0)
    end

    accBonus = accBonus - (params.flashPenalty or 0)

    return accBonus, evaBonus
end

-- Entity host: status/merit/JP/geometry/Flash reads → pure.
---@param attacker CBaseEntity
---@param target CBaseEntity
---@param isWeaponskill boolean
---@return number, number
xi.combat.physicalHitRate.getHitRateModifiers = function(attacker, target, isWeaponskill, isRanged)
    local flourishEffect = attacker:getStatusEffect(xi.effect.BUILDING_FLOURISH)
    local hasFlourish = flourishEffect ~= nil
    local flourishPower = 0
    local flourishSubPower = 0

    if hasFlourish then
        flourishPower = flourishEffect:getPower()
        flourishSubPower = flourishEffect:getSubPower()
    end

    local hasInnin = attacker:hasStatusEffect(xi.effect.INNIN)
    local inninPower = 0

    if hasInnin then
        inninPower = attacker:getStatusEffect(xi.effect.INNIN):getPower()
    end

    local hasYonin = attacker:hasStatusEffect(xi.effect.YONIN)
    local yoninPower = 0

    if hasYonin then
        yoninPower = attacker:getStatusEffect(xi.effect.YONIN):getPower()
    end

    local isBehind23 = attacker:isBehind(target, xi.combat.physicalHitRate.behindAngle)
    local isFacing64 = attacker:isFacing(target, xi.combat.physicalHitRate.facingConeYonin)

    return xi.combat.physicalHitRate.hitRateModifiersFromParams({
        isRanged                     = isRanged,
        isWeaponskill                = isWeaponskill,
        hasBuildingFlourish          = hasFlourish,
        buildingFlourishPower        = flourishPower,
        buildingFlourishSubPower     = flourishSubPower,
        hasInnin                     = hasInnin,
        inninPower                   = inninPower,
        inninJP                      = target:getJobPointLevel(xi.jp.INNIN_EFFECT),
        isBehind23                   = isBehind23,
        attackerIsPC                 = attacker:isPC(),
        attackerIsFacing             = attacker:isFacing(target),
        attackerClosedPositionMerit  = attacker:getMerit(xi.merit.CLOSED_POSITION),
        hasAmbushTrait               = attacker:hasTrait(xi.trait.AMBUSH),
        ambushMerit                  = attacker:getMerit(xi.merit.AMBUSH),
        hasYonin                     = hasYonin,
        yoninPower                   = yoninPower,
        yoninJP                      = target:getJobPointLevel(xi.jp.YONIN_EFFECT),
        isFacing64                   = isFacing64,
        targetIsPC                   = target:isPC(),
        targetIsFacing               = target:isFacing(attacker),
        targetClosedPositionMerit    = target:getMerit(xi.merit.CLOSED_POSITION),
        flashPenalty                 = xi.combat.physicalHitRate.getFlashPenalty(attacker),
    })
end

-----------------------------------
-- Pure: full melee / ranged hit-rate products (slice 6763 / 0838)
-- Parity: internal/physhitrate MeleeHitRate / RangedHitRate
-----------------------------------

-- params: acc, eva, bonus, accBonus, evaBonus, cap,
--   applyLevelCorrection, attackerLevel, defenderLevel, attackerIsPC, attackerIsAvatar
xi.combat.physicalHitRate.meleeHitRateFromParams = function(params)
    params = params or {}
    local acc = (params.acc or 0) + (params.bonus or 0) + (params.accBonus or 0)
    local eva = (params.eva or 0) + (params.evaBonus or 0)
    acc = xi.combat.physicalHitRate.levelCorrectedAccuracy(
        acc,
        params.attackerLevel or 0,
        params.defenderLevel or 0,
        params.applyLevelCorrection,
        params.attackerIsPC,
        params.attackerIsAvatar
    )
    local hitrate = xi.combat.physicalHitRate.accuracyEvasionToHitRate(acc, eva)
    return xi.combat.physicalHitRate.clampMeleeHitRate(hitrate, params.cap or xi.combat.physicalHitRate.capNonPC)
end

-- params: acc, eva, bonus, accBonus, evaBonus, distancePenalty, distance,
--   applyLevelCorrection, attackerLevel, defenderLevel, attackerIsPC, attackerIsAvatar
xi.combat.physicalHitRate.rangedHitRateFromParams = function(params)
    params = params or {}
    if (params.distance or 0) > xi.combat.physicalHitRate.maxRangedDistance then
        return 0
    end

    local acc = (params.acc or 0) + (params.bonus or 0) + (params.accBonus or 0) - (params.distancePenalty or 0)
    local eva = (params.eva or 0) + (params.evaBonus or 0)
    acc = xi.combat.physicalHitRate.levelCorrectedAccuracy(
        acc,
        params.attackerLevel or 0,
        params.defenderLevel or 0,
        params.applyLevelCorrection,
        params.attackerIsPC,
        params.attackerIsAvatar
    )
    local hitrate = xi.combat.physicalHitRate.accuracyEvasionToHitRate(acc, eva)
    return xi.combat.physicalHitRate.clampRangedHitRate(hitrate)
end

---@param attacker CBaseEntity
---@param target CBaseEntity
---@param bonus number
---@param slot xi.attackAnimation
---@param isWeaponskill boolean
---@return number
-- Host residual: ACC/EVA/cap/modifier entity reads.
-- Pure product: meleeHitRateFromParams (slice 6763).
xi.combat.physicalHitRate.getPhysicalHitRate = function(attacker, target, bonus, slot, isWeaponskill)
    local hitRateCap = xi.combat.physicalHitRate.getPhysicalHitRateCap(attacker, slot)
    local accBonus, evaBonus = xi.combat.physicalHitRate.getHitRateModifiers(attacker, target, isWeaponskill, false)

    return xi.combat.physicalHitRate.meleeHitRateFromParams({
        acc                  = attacker:getACC(slot), -- TODO: clamp slot for 0, 1, 2
        eva                  = target:getEVA(),
        bonus                = bonus or 0,
        accBonus             = accBonus,
        evaBonus             = evaBonus,
        cap                  = hitRateCap,
        applyLevelCorrection = xi.data.levelCorrection.isLevelCorrectedZone(attacker),
        attackerLevel        = attacker:getMainLvl(),
        defenderLevel        = target:getMainLvl(),
        attackerIsPC         = attacker:isPC(),
        attackerIsAvatar     = attacker:isAvatar(),
    })
end

---@param attacker CBaseEntity
---@param target CBaseEntity
---@param bonus number
---@param isWeaponskill boolean
---@return number
-- Host residual: RACC/EVA/distance/modifier entity reads.
-- Pure product: rangedHitRateFromParams (slice 6763).
xi.combat.physicalHitRate.getRangedHitRate = function(attacker, target, bonus, isWeaponskill)
    local accBonus, evaBonus = xi.combat.physicalHitRate.getHitRateModifiers(attacker, target, isWeaponskill, true)
    local distancePenalty = 0
    if attacker:checkDistance(target) <= xi.combat.physicalHitRate.maxRangedDistance then
        distancePenalty = xi.combat.ranged.accuracyDistancePenalty(attacker, target)
    end

    return xi.combat.physicalHitRate.rangedHitRateFromParams({
        acc                  = attacker:getRACC(),
        eva                  = target:getEVA(),
        bonus                = bonus or 0,
        accBonus             = accBonus,
        evaBonus             = evaBonus,
        distancePenalty      = distancePenalty,
        distance             = attacker:checkDistance(target),
        applyLevelCorrection = xi.data.levelCorrection.isLevelCorrectedZone(attacker),
        attackerLevel        = attacker:getMainLvl(),
        defenderLevel        = target:getMainLvl(),
        attackerIsPC         = attacker:isPC(),
        attackerIsAvatar     = attacker:isAvatar(),
    })
end
