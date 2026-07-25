-----------------------------------
-- Samurai Job Utilities
-- Dual-wired pure inject forms (slice 6734 / 0887):
--   checks (2H/combat/Shikikoyo), oneHour/thirdEye recast,
--   WardingCircle/Hasso/Meditate/ThirdEye/Shikikoyo/Hamanoha/Konzen/BladeBash
-- Parity: internal/samurai
-----------------------------------
require('scripts/globals/ability')
require('scripts/globals/jobpoints')
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.samurai = xi.job_utils.samurai or {}

-----------------------------------
-- Pure inject pins (internal/samurai)
-----------------------------------
xi.job_utils.samurai.wardingCircleMainPower       = 15
xi.job_utils.samurai.wardingCircleSubPower        = 5
xi.job_utils.samurai.wardingCircleBaseDuration    = 180
xi.job_utils.samurai.hassoDuration                = 300
xi.job_utils.samurai.seiganDuration               = 300
xi.job_utils.samurai.hassoLevelDivisor            = 7
xi.job_utils.samurai.meditateSubAmount            = 12
xi.job_utils.samurai.meditateMainBase             = 20
xi.job_utils.samurai.meditateJPPerLevel           = 5
xi.job_utils.samurai.meditateBaseDuration         = 15
xi.job_utils.samurai.meditateTick                 = 3
xi.job_utils.samurai.thirdEyeDuration             = 30
xi.job_utils.samurai.shikikoyoKeepTP              = 1000
xi.job_utils.samurai.shikikoyoMeritBaseline       = 12
xi.job_utils.samurai.shikikoyoTPCap               = 3000
xi.job_utils.samurai.meikyoShisuiDuration         = 30
xi.job_utils.samurai.meikyoShisuiTPGrant          = 3000
xi.job_utils.samurai.yaegasumiPower               = 12
xi.job_utils.samurai.yaegasumiDuration            = 45
xi.job_utils.samurai.sekkanokiDuration            = 60
xi.job_utils.samurai.sekkanokiPower               = 1
xi.job_utils.samurai.sengikoriPower               = 25
xi.job_utils.samurai.sengikoriDuration            = 60
xi.job_utils.samurai.hagakurePower                = 400
xi.job_utils.samurai.hagakureSubPower             = 1000
xi.job_utils.samurai.hagakureDuration             = 60
xi.job_utils.samurai.hamanohaBaseDuration         = 180
xi.job_utils.samurai.hamanohaPower                = 12
xi.job_utils.samurai.konzenMissInfo               = 1
xi.job_utils.samurai.konzenHitInfo                = 5
xi.job_utils.samurai.konzenHitReturn              = 3
xi.job_utils.samurai.konzenMissReturn             = 0
xi.job_utils.samurai.konzenDefaultAnimation       = 37
xi.job_utils.samurai.konzenChainboundPower        = 2
xi.job_utils.samurai.konzenChainboundDuration     = 10
xi.job_utils.samurai.konzenChainboundSubPower     = 1
xi.job_utils.samurai.bladeBashPlagueBase          = 15
xi.job_utils.samurai.bladeBashPlaguePower         = 5
xi.job_utils.samurai.bladeBashStunDuration        = 6
xi.job_utils.samurai.bladeBashStunPower           = 1
xi.job_utils.samurai.oneHourRecastSecondsPerMod   = 60
xi.job_utils.samurai.msgCannotPerformTarg         = 72
xi.job_utils.samurai.msgJANoEffect                = 156
xi.job_utils.samurai.msgNotEnoughTP               = 192
xi.job_utils.samurai.msgNeeds2HWeapon             = 307
xi.job_utils.samurai.msgRequiresCombat            = 525

-- Pure check: 2H weapon required (Hasso/Seigan/Blade Bash).
-- returns: msg, ok
xi.job_utils.samurai.checkTwoHandedWeaponFromParams = function(params)
    params = params or {}
    if not params.isTwoHanded then
        return xi.job_utils.samurai.msgNeeds2HWeapon, false
    end

    return 0, true
end

-- Pure check: Konzen in combat (animation == 1).
xi.job_utils.samurai.checkKonzenIttaiFromParams = function(params)
    params = params or {}
    if not params.inCombat then
        return xi.job_utils.samurai.msgRequiresCombat, false
    end

    return 0, true
end

-- Pure check: Shikikoyo self/TP.
xi.job_utils.samurai.checkShikikoyoFromParams = function(params)
    params = params or {}
    if params.selfTarget then
        return xi.job_utils.samurai.msgCannotPerformTarg, false
    end

    if (params.playerTP or 0) < xi.job_utils.samurai.shikikoyoKeepTP then
        return xi.job_utils.samurai.msgNotEnoughTP, false
    end

    return 0, true
end

-- Pure one-hour recast.
xi.job_utils.samurai.oneHourRecastFromParams = function(params)
    params = params or {}
    return math.max(0, (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.samurai.oneHourRecastSecondsPerMod)
end

-- Pure Third Eye recast (half when Seigan + 2H).
xi.job_utils.samurai.thirdEyeRecastFromParams = function(params)
    params = params or {}
    local recast = params.abilityRecast or 0
    if params.hasSeigan and params.isTwoHanded then
        return math.floor(recast / 2)
    end

    return recast
end

-- Pure Warding Circle power/duration.
xi.job_utils.samurai.wardingCirclePowerFromParams = function(params)
    params = params or {}
    local power = xi.job_utils.samurai.wardingCircleSubPower
    if params.mainJobSAM then
        power = xi.job_utils.samurai.wardingCircleMainPower
    end

    return power + (params.potencyMod or 0)
end

xi.job_utils.samurai.wardingCircleDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.samurai.wardingCircleBaseDuration + (params.durationMod or 0)
end

-- Pure Hasso STR boost (may be fractional).
xi.job_utils.samurai.hassoSTRBoostFromParams = function(params)
    params = params or {}
    if params.mainJobSAM then
        return (params.mainLvl or 0) / xi.job_utils.samurai.hassoLevelDivisor + (params.hassoJP or 0)
    end

    if params.subJobSAM then
        return (params.subLvl or 0) / xi.job_utils.samurai.hassoLevelDivisor
    end

    return 0
end

-- Pure Meditate amount/duration/tick.
-- returns: { amount, duration, tick }
xi.job_utils.samurai.meditateFromParams = function(params)
    params = params or {}
    local amount = xi.job_utils.samurai.meditateSubAmount
    if params.mainJobSAM then
        amount = xi.job_utils.samurai.meditateMainBase
            + (params.meditateJP or 0) * xi.job_utils.samurai.meditateJPPerLevel
    end

    return {
        amount   = amount,
        duration = xi.job_utils.samurai.meditateBaseDuration + (params.durationMod or 0),
        tick     = xi.job_utils.samurai.meditateTick,
    }
end

-- Pure Third Eye apply gate.
xi.job_utils.samurai.thirdEyeAppliesFromParams = function(params)
    params = params or {}
    return not params.hasCopyImage and not params.hasBlink
end

-- Pure Shikikoyo TP transfer.
xi.job_utils.samurai.shikikoyoTransferFromParams = function(params)
    params = params or {}
    local scale = 1 + ((params.meritValue or 0) - xi.job_utils.samurai.shikikoyoMeritBaseline) / 100
    local pTP = ((params.playerTP or 0) - xi.job_utils.samurai.shikikoyoKeepTP) * scale
    local maxTransfer = xi.job_utils.samurai.shikikoyoTPCap - (params.targetTP or 0)
    if maxTransfer < 0 then
        return 0
    end

    if pTP < 0 then
        pTP = 0
    end

    if pTP > maxTransfer then
        pTP = maxTransfer
    end

    return pTP
end

-- Pure Hamanoha duration.
xi.job_utils.samurai.hamanohaDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.samurai.hamanohaBaseDuration + (params.hamanohaJP or 0)
end

-- Pure Blade Bash plague base duration before resist.
xi.job_utils.samurai.bladeBashPlagueDurationBase = function(bladeBashMerit)
    return xi.job_utils.samurai.bladeBashPlagueBase + (bladeBashMerit or 0)
end

-- Pure Konzen animation table.
xi.job_utils.samurai.konzenAnimation = function(weaponSkill)
    local animationTable =
    {
        [xi.skill.AXE         ] = 28,
        [xi.skill.DAGGER      ] = 36,
        [xi.skill.SWORD       ] = 36,
        [xi.skill.NONE        ] = 37,
        [xi.skill.HAND_TO_HAND] = 37,
        [xi.skill.CLUB        ] = 39,
        [xi.skill.GREAT_AXE   ] = 40,
        [xi.skill.SCYTHE      ] = 40,
        [xi.skill.GREAT_SWORD ] = 41,
        [xi.skill.POLEARM     ] = 42,
        [xi.skill.KATANA      ] = 43,
        [xi.skill.GREAT_KATANA] = 44,
        [xi.skill.STAFF       ] = 45,
    }

    return animationTable[weaponSkill or 0] or xi.job_utils.samurai.konzenDefaultAnimation
end

-- Pure Blade Bash animation table.
xi.job_utils.samurai.bladeBashAnimation = function(weaponSkill)
    local animationTable =
    {
        [xi.skill.GREAT_SWORD ] = 201,
        [xi.skill.GREAT_KATANA] = 201,
        [xi.skill.GREAT_AXE   ] = 202,
        [xi.skill.SCYTHE      ] = 202,
        [xi.skill.STAFF       ] = 202,
        [xi.skill.POLEARM     ] = 203,
    }

    return animationTable[weaponSkill or 0] or 0
end

-- Pure Konzen Ittai plan.
-- returns: { animation, info, applied, returnValue, noEffectMsg }
xi.job_utils.samurai.konzenIttaiFromParams = function(params)
    params = params or {}
    local out =
    {
        animation   = xi.job_utils.samurai.konzenAnimation(params.weaponSkill),
        info        = xi.job_utils.samurai.konzenMissInfo,
        applied     = false,
        returnValue = xi.job_utils.samurai.konzenMissReturn,
        noEffectMsg = false,
    }

    if not params.hasChainbound and not params.hasSkillchain then
        out.info        = xi.job_utils.samurai.konzenHitInfo
        out.applied     = true
        out.returnValue = xi.job_utils.samurai.konzenHitReturn
        return out
    end

    out.noEffectMsg = true
    return out
end

-----------------------------------
-- Ability Check Functions (hosts → pure)
-----------------------------------

xi.job_utils.samurai.checkMeikyoShisui = function(player, target, ability)
    ability:setRecast(xi.job_utils.samurai.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.samurai.checkYaegasumi = function(player, target, ability)
    ability:setRecast(xi.job_utils.samurai.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.samurai.checkThirdEye = function(player, target, ability)
    ability:setRecast(xi.job_utils.samurai.thirdEyeRecastFromParams({
        abilityRecast = ability:getRecast(),
        hasSeigan     = player:hasStatusEffect(xi.effect.SEIGAN),
        isTwoHanded   = player:isWeaponTwoHanded(),
    }))

    return 0, 0
end

xi.job_utils.samurai.checkHasso = function(player, target, ability)
    local msg = xi.job_utils.samurai.checkTwoHandedWeaponFromParams({
        isTwoHanded = player:isWeaponTwoHanded(),
    })
    return msg, 0
end

xi.job_utils.samurai.checkSeigan = function(player, target, ability)
    local msg = xi.job_utils.samurai.checkTwoHandedWeaponFromParams({
        isTwoHanded = player:isWeaponTwoHanded(),
    })
    return msg, 0
end

xi.job_utils.samurai.checkKonzenIttai = function(player, target, ability)
    local msg = xi.job_utils.samurai.checkKonzenIttaiFromParams({
        inCombat = player:getAnimation() == 1,
    })
    return msg, 0
end

xi.job_utils.samurai.checkBladeBash = function(player, target, ability)
    local msg = xi.job_utils.samurai.checkTwoHandedWeaponFromParams({
        isTwoHanded = player:isWeaponTwoHanded(),
    })
    return msg, 0
end

xi.job_utils.samurai.checkShikikoyo = function(player, target, ability)
    local msg = xi.job_utils.samurai.checkShikikoyoFromParams({
        selfTarget = player:getID() == target:getID(),
        playerTP   = player:getTP(),
    })
    return msg, 0
end

-----------------------------------
-- Ability Use Functions (hosts → pure)
-----------------------------------

xi.job_utils.samurai.useMeikyoShisui = function(player, target, ability)
    player:addStatusEffect(xi.effect.MEIKYO_SHISUI, {
        power    = 1,
        duration = xi.job_utils.samurai.meikyoShisuiDuration,
        origin   = player,
    })
    player:addTP(xi.job_utils.samurai.meikyoShisuiTPGrant)

    return 0
end

xi.job_utils.samurai.useYaegasumi = function(player, target, ability)
    player:addStatusEffect(xi.effect.YAEGASUMI, {
        power    = xi.job_utils.samurai.yaegasumiPower,
        duration = xi.job_utils.samurai.yaegasumiDuration,
        origin   = player,
    })

    return xi.effect.YAEGASUMI
end

xi.job_utils.samurai.useWardingCircle = function(player, target, ability)
    local power = xi.job_utils.samurai.wardingCirclePowerFromParams({
        mainJobSAM = player:getMainJob() == xi.job.SAM,
        potencyMod = player:getMod(xi.mod.WARDING_CIRCLE_POTENCY),
    })
    local duration = xi.job_utils.samurai.wardingCircleDurationFromParams({
        durationMod = player:getMod(xi.mod.WARDING_CIRCLE_DURATION),
    })

    target:addStatusEffect(xi.effect.WARDING_CIRCLE, { power = power, duration = duration, origin = player })

    return xi.effect.WARDING_CIRCLE
end

xi.job_utils.samurai.useThirdEye = function(player, target, ability)
    if not xi.job_utils.samurai.thirdEyeAppliesFromParams({
        hasCopyImage = player:hasStatusEffect(xi.effect.COPY_IMAGE),
        hasBlink     = player:hasStatusEffect(xi.effect.BLINK),
    }) then
        -- Returns "no effect" message when Copy Image is active when Third Eye is used.
        ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
    else
        player:addStatusEffect(xi.effect.THIRD_EYE, {
            duration = xi.job_utils.samurai.thirdEyeDuration,
            origin   = player,
        }) -- Power keeps track of procs
    end

    return xi.effect.THIRD_EYE
end

xi.job_utils.samurai.useHasso = function(player, target, ability)
    local strboost = xi.job_utils.samurai.hassoSTRBoostFromParams({
        mainJobSAM = target:getMainJob() == xi.job.SAM,
        subJobSAM  = target:getSubJob() == xi.job.SAM,
        mainLvl    = target:getMainLvl(),
        subLvl     = target:getSubLvl(),
        hassoJP    = target:getJobPointLevel(xi.jp.HASSO_EFFECT),
    })

    if strboost > 0 then
        target:delStatusEffect(xi.effect.HASSO)
        target:delStatusEffect(xi.effect.SEIGAN)
        target:addStatusEffect(xi.effect.HASSO, {
            power    = strboost,
            duration = xi.job_utils.samurai.hassoDuration,
            origin   = player,
        })
    end

    return xi.effect.HASSO
end

xi.job_utils.samurai.useMeditate = function(player, target, ability)
    local med = xi.job_utils.samurai.meditateFromParams({
        mainJobSAM  = player:getMainJob() == xi.job.SAM,
        meditateJP  = player:getJobPointLevel(xi.jp.MEDITATE_EFFECT),
        durationMod = player:getMod(xi.mod.MEDITATE_DURATION),
    })

    player:addStatusEffect(xi.effect.MEDITATE, {
        power    = med.amount,
        duration = med.duration,
        origin   = player,
        tick     = med.tick,
        icon     = 0,
    })

    return xi.effect.MEDITATE
end

xi.job_utils.samurai.useSeigan = function(player, target, ability)
    if target:isWeaponTwoHanded() then
        target:delStatusEffect(xi.effect.HASSO)
        target:delStatusEffect(xi.effect.SEIGAN)
        target:addStatusEffect(xi.effect.SEIGAN, {
            duration = xi.job_utils.samurai.seiganDuration,
            origin   = player,
        })
    end

    return xi.effect.SEIGAN
end

xi.job_utils.samurai.useSekkanoki = function(player, target, ability)
    target:delStatusEffect(xi.effect.SEKKANOKI)
    target:addStatusEffect(xi.effect.SEKKANOKI, {
        power    = xi.job_utils.samurai.sekkanokiPower,
        duration = xi.job_utils.samurai.sekkanokiDuration,
        origin   = player,
    })

    return xi.effect.SEKKANOKI
end

xi.job_utils.samurai.useKonzenIttai = function(player, target, ability, action)
    local plan = xi.job_utils.samurai.konzenIttaiFromParams({
        weaponSkill    = player:getWeaponSkillType(xi.slot.MAIN),
        hasChainbound  = target:hasStatusEffect(xi.effect.CHAINBOUND, 0),
        hasSkillchain  = target:hasStatusEffect(xi.effect.SKILLCHAIN, 0),
    })

    if plan.applied then
        target:addStatusEffect(xi.effect.CHAINBOUND, {
            power    = xi.job_utils.samurai.konzenChainboundPower,
            duration = xi.job_utils.samurai.konzenChainboundDuration,
            origin   = player,
            icon     = 0,
            subPower = xi.job_utils.samurai.konzenChainboundSubPower,
        })
    elseif plan.noEffectMsg then
        ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
    end

    action:setAnimation(target:getID(), plan.animation)
    action:info(target:getID(), plan.info)

    return plan.returnValue
end

xi.job_utils.samurai.useBladeBash = function(player, target, ability, action)
    -- Stun
    if
        not xi.data.statusEffect.isTargetImmune(target, xi.effect.STUN, xi.element.THUNDER) and
        not xi.data.statusEffect.isTargetResistant(player, target, xi.effect.STUN) and
        not xi.data.statusEffect.isEffectNullified(target, xi.effect.STUN, 0)
    then
        local resistanceRate = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, xi.skillRank.A_PLUS, xi.element.THUNDER, xi.mod.INT, xi.effect.STUN, 0)
        if xi.data.statusEffect.isResistRateSuccessfull(xi.effect.STUN, resistanceRate, 0) then
            target:addStatusEffect(xi.effect.STUN, {
                power    = xi.job_utils.samurai.bladeBashStunPower,
                duration = xi.job_utils.samurai.bladeBashStunDuration * resistanceRate,
                origin   = player,
            })
        end
    end

    -- Plague
    if
        not xi.data.statusEffect.isTargetImmune(target, xi.effect.PLAGUE, xi.element.FIRE) and
        not xi.data.statusEffect.isTargetResistant(player, target, xi.effect.PLAGUE) and
        not xi.data.statusEffect.isEffectNullified(target, xi.effect.PLAGUE, 0)
    then
        local resistanceRate = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, xi.skillRank.A_PLUS, xi.element.FIRE, xi.mod.INT, xi.effect.PLAGUE, 0)
        if xi.data.statusEffect.isResistRateSuccessfull(xi.effect.PLAGUE, resistanceRate, 0) then
            local duration = xi.job_utils.samurai.bladeBashPlagueDurationBase(player:getMerit(xi.merit.BLADE_BASH)) * resistanceRate
            target:addStatusEffect(xi.effect.PLAGUE, {
                power    = xi.job_utils.samurai.bladeBashPlaguePower,
                duration = duration,
                origin   = player,
            })
        end
    end

    -- Animation
    local animation = xi.job_utils.samurai.bladeBashAnimation(player:getWeaponSkillType(xi.slot.MAIN))
    action:setAnimation(target:getID(), animation)

    ability:setMsg(xi.msg.basic.JA_DAMAGE)

    -- Blade Bash does not deal damage
    return 0
end

xi.job_utils.samurai.useShikikoyo = function(player, target, ability, action)
    local pTP = xi.job_utils.samurai.shikikoyoTransferFromParams({
        playerTP   = player:getTP(),
        targetTP   = target:getTP(),
        meritValue = player:getMerit(xi.merit.SHIKIKOYO),
    })

    player:setTP(xi.job_utils.samurai.shikikoyoKeepTP)
    target:setTP(target:getTP() + pTP)

    return pTP
end

xi.job_utils.samurai.useSengikori = function(player, target, ability)
    player:addStatusEffect(xi.effect.SENGIKORI, {
        power    = xi.job_utils.samurai.sengikoriPower,
        duration = xi.job_utils.samurai.sengikoriDuration,
        origin   = player,
    })

    return xi.effect.SENGIKORI
end

xi.job_utils.samurai.useHamanoha = function(player, target, ability)
    local duration = xi.job_utils.samurai.hamanohaDurationFromParams({
        hamanohaJP = target:getJobPointLevel(xi.jp.HAMANOHA_DURATION),
    })

    target:addStatusEffect(xi.effect.HAMANOHA, {
        power    = xi.job_utils.samurai.hamanohaPower,
        duration = duration,
        origin   = player,
    })

    return xi.effect.HAMANOHA
end

xi.job_utils.samurai.useHagakure = function(player, target, ability)
    player:delStatusEffect(xi.effect.HAGAKURE)
    player:addStatusEffect(xi.effect.HAGAKURE, {
        power    = xi.job_utils.samurai.hagakurePower,
        duration = xi.job_utils.samurai.hagakureDuration,
        origin   = player,
        subPower = xi.job_utils.samurai.hagakureSubPower,
    })

    return xi.effect.HAGAKURE
end
