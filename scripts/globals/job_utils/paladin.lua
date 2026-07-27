-----------------------------------
-- Paladin Job Utilities
-- Dual-wired pure inject forms (slice 6739):
--   one-hour recast, Cover bonus/duration, Divine Emblem/Fealty/Holy Circle,
--   Intervene/Shield Bash base damage, Palisade/Rampart/Sentinel/Sepulcher,
--   Chivalry MP product
-- Parity: internal/paladin
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.paladin = xi.job_utils.paladin or {}

-----------------------------------
-- Pure inject pins (internal/paladin)
-----------------------------------
xi.job_utils.paladin.coverBaseDuration            = 15
xi.job_utils.paladin.coverBonusTimeCap            = 15
xi.job_utils.paladin.divineEmblemBasePower        = 50
xi.job_utils.paladin.divineEmblemDuration         = 60
xi.job_utils.paladin.fealtyBaseDuration           = 60
xi.job_utils.paladin.fealtyMeritUnit              = 5
xi.job_utils.paladin.holyCircleMainPower          = 15
xi.job_utils.paladin.holyCircleSubPower           = 5
xi.job_utils.paladin.holyCircleBaseDuration       = 180
xi.job_utils.paladin.interveneLvlScale            = 3.36
xi.job_utils.paladin.interveneDuration            = 30
xi.job_utils.paladin.invincibleDuration           = 30
xi.job_utils.paladin.majestyPower                 = 25
xi.job_utils.paladin.majestyDuration              = 180
xi.job_utils.paladin.palisadeBasePower            = 30
xi.job_utils.paladin.palisadeDuration             = 60
xi.job_utils.paladin.rampartPower                 = 2500
xi.job_utils.paladin.rampartBaseDuration          = 30 -- production LSB pin
xi.job_utils.paladin.sentinelBasePct              = 90
xi.job_utils.paladin.sentinelBaseDuration         = 30
xi.job_utils.paladin.sentinelTick                 = 3
xi.job_utils.paladin.sepulcherPower               = 20
xi.job_utils.paladin.sepulcherBaseDuration        = 180
xi.job_utils.paladin.shieldBashLvlScale           = 0.273
xi.job_utils.paladin.chivalryBaseMP               = 0.05
xi.job_utils.paladin.chivalryMNDScale             = 0.0015
xi.job_utils.paladin.oneHourRecastSecondsPerMod   = 60
xi.job_utils.paladin.msgCannotPerformTarg         = 72
xi.job_utils.paladin.msgRequiresShield            = 199
xi.job_utils.paladin.msgCannotOnThatTarg          = 155

-- Pure: OneHourRecast
xi.job_utils.paladin.oneHourRecastFromParams = function(params)
    params = params or {}
    local r = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.paladin.oneHourRecastSecondsPerMod
    if r < 0 then
        return 0
    end

    return r
end

-- Pure: CoverBonusTime
xi.job_utils.paladin.coverBonusTimeFromParams = function(params)
    params = params or {}
    local v = math.floor(((params.playerVIT or 0) + (params.playerMND or 0) - (params.targetVIT or 0) * 2) / 4)
    if v < 0 then
        return 0
    end

    if v > xi.job_utils.paladin.coverBonusTimeCap then
        return xi.job_utils.paladin.coverBonusTimeCap
    end

    return v
end

-- Pure: CoverDuration
xi.job_utils.paladin.coverDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.paladin.coverBaseDuration
        + (params.bonusTime or 0)
        + (params.merit or 0)
        + (params.coverDurationMod or 0)
        + (params.jp or 0)
end

-- Pure: DivineEmblemPower
xi.job_utils.paladin.divineEmblemPowerFromParams = function(enhances)
    return xi.job_utils.paladin.divineEmblemBasePower + (enhances or 0)
end

-- Pure: FealtyDuration
-- params: merits (raw getMerit), enhancesFealty
xi.job_utils.paladin.fealtyDurationFromParams = function(params)
    params = params or {}
    local merits = params.merits or 0
    local m = merits - xi.job_utils.paladin.fealtyMeritUnit
    local enh = math.floor(merits / xi.job_utils.paladin.fealtyMeritUnit) * (params.enhancesFealty or 0)
    return xi.job_utils.paladin.fealtyBaseDuration + m + enh
end

-- Pure: HolyCirclePower
xi.job_utils.paladin.holyCirclePowerFromParams = function(params)
    params = params or {}
    local p = xi.job_utils.paladin.holyCircleMainPower
    if not params.isMain then
        p = xi.job_utils.paladin.holyCircleSubPower
    end

    return p + (params.potency or 0)
end

-- Pure: HolyCircleDuration
xi.job_utils.paladin.holyCircleDurationFromParams = function(mod)
    return xi.job_utils.paladin.holyCircleBaseDuration + (mod or 0)
end

-- Pure: InterveneDamage base (before shield size / JP)
xi.job_utils.paladin.interveneDamageFromParams = function(mainLvl)
    return math.floor((mainLvl or 0) * xi.job_utils.paladin.interveneLvlScale)
end

-- Pure: PalisadePower
xi.job_utils.paladin.palisadePowerFromParams = function(jp)
    return xi.job_utils.paladin.palisadeBasePower + (jp or 0)
end

-- Pure: RampartDuration (LSB production base 30)
xi.job_utils.paladin.rampartDurationFromParams = function(mod)
    return xi.job_utils.paladin.rampartBaseDuration + (mod or 0)
end

-- Pure: SentinelPower
xi.job_utils.paladin.sentinelPowerFromParams = function(sentinelEffect)
    return (xi.job_utils.paladin.sentinelBasePct + (sentinelEffect or 0)) * 100
end

-- Pure: SentinelDuration
-- params: enhGuardian (ENHANCES_GUARDIAN * (guardian/19) precomputed by host)
xi.job_utils.paladin.sentinelDurationFromParams = function(enhGuardian)
    return xi.job_utils.paladin.sentinelBaseDuration + (enhGuardian or 0)
end

-- Pure: SepulcherDuration
xi.job_utils.paladin.sepulcherDurationFromParams = function(jp)
    return xi.job_utils.paladin.sepulcherBaseDuration + (jp or 0)
end

-- Pure: ChivalryMP
-- params: tp, mnd, enhancesChivalry, merits (raw getMerit)
xi.job_utils.paladin.chivalryMPFromParams = function(params)
    params = params or {}
    local base = xi.job_utils.paladin.chivalryBaseMP + (params.enhancesChivalry or 0) / 100
    local meritsAdj = (params.merits or 0) - xi.job_utils.paladin.fealtyMeritUnit -- same unit 5
    local tp = params.tp or 0
    local mnd = params.mnd or 0
    return (tp * base) + (xi.job_utils.paladin.chivalryMNDScale * tp * mnd) * ((100 + meritsAdj) / 100)
end

-- Pure: ShieldBashBaseDamage
xi.job_utils.paladin.shieldBashBaseDamageFromParams = function(mainLvl)
    return math.floor((mainLvl or 0) * xi.job_utils.paladin.shieldBashLvlScale)
end

-----------------------------------
-- Ability Check Functions
-----------------------------------
xi.job_utils.paladin.checkCover = function(player, target, ability)
    if
        target == nil or
        target:getID() == player:getID() or
        not target:isPC()
    then
        return xi.msg.basic.CANNOT_PERFORM_TARG, 0
    else
        return 0, 0
    end
end

xi.job_utils.paladin.checkIntervene = function(player, target, ability)
    if player:getShieldSize() == 0 then
        return xi.msg.basic.REQUIRES_SHIELD, 0
    else
        ability:setRecast(xi.job_utils.paladin.oneHourRecastFromParams({
            abilityRecast    = ability:getRecast(),
            oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
        }))

        return 0, 0
    end
end

xi.job_utils.paladin.checkInvincible = function(player, target, ability)
    local jpValue = player:getJobPointLevel(xi.jp.INVINCIBLE_EFFECT)

    ability:setVE(ability:getVE() + 100 * jpValue)
    ability:setRecast(xi.job_utils.paladin.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.paladin.checkSepulcher = function(player, target, ability)
    if target:isUndead() then
        return 0, 0
    else
        return xi.msg.basic.CANNOT_ON_THAT_TARG, 0
    end
end

xi.job_utils.paladin.checkShieldBash = function(player, target, ability)
    if player:getShieldSize() == 0 then
        return xi.msg.basic.REQUIRES_SHIELD, 0
    else
        return 0, 0
    end
end

-----------------------------------
-- Ability Use Functions
-----------------------------------
xi.job_utils.paladin.useChivalry = function(player, target, ability, action)
    local amount = xi.job_utils.paladin.chivalryMPFromParams({
        tp               = target:getTP(),
        mnd              = target:getStat(xi.mod.MND),
        enhancesChivalry = player:getMod(xi.mod.ENHANCES_CHIVALRY),
        merits           = player:getMerit(xi.merit.CHIVALRY),
    })

    target:setTP(0)

    return target:addMP(amount)
end

xi.job_utils.paladin.useCover = function(player, target, ability)
    local bonusTime = xi.job_utils.paladin.coverBonusTimeFromParams({
        playerVIT = player:getStat(xi.mod.VIT),
        playerMND = player:getStat(xi.mod.MND),
        targetVIT = target:getStat(xi.mod.VIT),
    })
    local duration = xi.job_utils.paladin.coverDurationFromParams({
        bonusTime         = bonusTime,
        merit             = player:getMerit(xi.merit.COVER_EFFECT_LENGTH),
        coverDurationMod  = player:getMod(xi.mod.COVER_DURATION),
        jp                = player:getJobPointLevel(xi.jp.COVER_DURATION),
    })

    player:addStatusEffect(xi.effect.COVER, {
        power    = player:getMod(xi.mod.COVER_TO_MP),
        duration = duration,
        origin   = player,
    })
    player:setLocalVar('COVER_ABILITY_TARGET', target:getID())
    ability:setMsg(xi.msg.basic.COVER_SUCCESS)
end

xi.job_utils.paladin.useDivineEmblem = function(player, target, ability)
    -- Divine Magic bonus damage handled in globals/magic.lua
    local power = xi.job_utils.paladin.divineEmblemPowerFromParams(
        player:getMod(xi.mod.ENHANCES_DIVINE_EMBLEM)
    )

    player:addStatusEffect(xi.effect.DIVINE_EMBLEM, {
        power    = power,
        duration = xi.job_utils.paladin.divineEmblemDuration,
        origin   = player,
    })

    return xi.effect.DIVINE_EMBLEM
end

xi.job_utils.paladin.useFealty = function(player, target, ability, action)
    local duration = xi.job_utils.paladin.fealtyDurationFromParams({
        merits          = player:getMerit(xi.merit.FEALTY),
        enhancesFealty  = player:getMod(xi.mod.ENHANCES_FEALTY),
    })

    player:addStatusEffect(xi.effect.FEALTY, {
        power    = 1,
        duration = duration,
        origin   = player,
    })

    return xi.effect.FEALTY
end

xi.job_utils.paladin.useHolyCircle = function(player, target, ability)
    -- TODO:
    -- Create Bonus vs Ecosystem handling
    -- https://www.bg-wiki.com/ffxi/Holy_Circle
    local duration = xi.job_utils.paladin.holyCircleDurationFromParams(
        player:getMod(xi.mod.HOLY_CIRCLE_DURATION)
    )
    local power = xi.job_utils.paladin.holyCirclePowerFromParams({
        isMain  = player:getMainJob() == xi.job.PLD,
        potency = player:getMod(xi.mod.HOLY_CIRCLE_POTENCY),
    })

    target:addStatusEffect(xi.effect.HOLY_CIRCLE, {
        power    = power,
        duration = duration,
        origin   = player,
    })

    return xi.effect.HOLY_CIRCLE
end

xi.job_utils.paladin.useIntervene = function(player, target, ability)
    -- TODO: Retail testing to determine damage
    local shieldSize = player:getShieldSize()
    local jpValue    = 1 + (player:getJobPointLevel(xi.jp.INTERVENE_EFFECT) / 100)
    local damage     = xi.job_utils.paladin.interveneDamageFromParams(player:getMainLvl())

    if shieldSize == 2 then
        damage = 13 + damage
    elseif shieldSize == 3 then
        damage = 40 + damage
    elseif shieldSize == 4 then
        damage = 67 + damage
    end

    damage = damage * jpValue

    target:addStatusEffect(xi.effect.INTERVENE, {
        power    = 1,
        duration = xi.job_utils.paladin.interveneDuration,
        origin   = player,
    })

    return damage
end

xi.job_utils.paladin.useInvincible = function(player, target, ability)
    player:addStatusEffect(xi.effect.INVINCIBLE, {
        power    = 1,
        duration = xi.job_utils.paladin.invincibleDuration,
        origin   = player,
    })

    return xi.effect.INVINCIBLE
end

xi.job_utils.paladin.useMajesty = function(player, target, ability)
    player:addStatusEffect(xi.effect.MAJESTY, {
        power    = xi.job_utils.paladin.majestyPower,
        duration = xi.job_utils.paladin.majestyDuration,
        origin   = player,
    })

    return xi.effect.MAJESTY
end

xi.job_utils.paladin.usePalisade = function(player, target, ability)
    local power = xi.job_utils.paladin.palisadePowerFromParams(
        player:getJobPointLevel(xi.jp.PALISADE_EFFECT)
    )

    player:addStatusEffect(xi.effect.PALISADE, {
        power    = power,
        duration = xi.job_utils.paladin.palisadeDuration,
        origin   = player,
    })

    return xi.effect.PALISADE
end

xi.job_utils.paladin.useRampart = function(player, target, ability)
    local duration = xi.job_utils.paladin.rampartDurationFromParams(
        player:getMod(xi.mod.RAMPART_DURATION)
    )

    target:addStatusEffect(xi.effect.RAMPART, {
        power    = xi.job_utils.paladin.rampartPower,
        duration = duration,
        origin   = player,
    })

    return xi.effect.RAMPART
end

xi.job_utils.paladin.useSentinel = function(player, target, ability)
    -- Whether feet have to be equipped before using ability, or if they can be swapped in
    -- is disputed.  Source used: http://wiki.bluegartr.com/bg/Sentinel
    local power       = xi.job_utils.paladin.sentinelPowerFromParams(player:getMod(xi.mod.SENTINEL_EFFECT))
    local guardian    = player:getMerit(xi.merit.GUARDIAN)
    local enhGuardian = player:getMod(xi.mod.ENHANCES_GUARDIAN) * (guardian / 19)
    local jpValue     = player:getJobPointLevel(xi.jp.SENTINEL_EFFECT)
    local duration    = xi.job_utils.paladin.sentinelDurationFromParams(enhGuardian)

    -- Sent as positive power because UINTs, man.
    player:addStatusEffect(xi.effect.SENTINEL, {
        power    = power,
        duration = duration,
        origin   = player,
        tick     = xi.job_utils.paladin.sentinelTick,
        subPower = guardian + jpValue,
    })

    return xi.effect.SENTINEL
end

xi.job_utils.paladin.useSepulcher = function(player, target, ability)
    local power    = xi.job_utils.paladin.sepulcherPower
    local duration = xi.job_utils.paladin.sepulcherDurationFromParams(
        player:getJobPointLevel(xi.jp.SEPULCHER_DURATION)
    )

    target:addStatusEffect(xi.effect.SEPULCHER, {
        power    = power,
        duration = duration,
        origin   = player,
    })
end

xi.job_utils.paladin.useShieldBash = function(player, target, ability)
    local shieldSize = player:getShieldSize()
    local jpValue    = player:getJobPointLevel(xi.jp.SHIELD_BASH_EFFECT)
    local damage     = xi.job_utils.paladin.shieldBashBaseDamageFromParams(player:getMainLvl())

    if shieldSize == 2 then
        damage = 13 + damage
    elseif shieldSize == 3 then
        damage = 40 + damage
    elseif shieldSize == 4 then
        damage = 67 + damage
    end

    -- Main job factors
    if player:getMainJob() ~= xi.job.PLD then
        damage = math.floor(damage / 2.5)
    else
        damage = math.floor(damage)
    end

    damage = damage + player:getMod(xi.mod.SHIELD_BASH) + (jpValue * 10)

    -- Apply stun effect
    if
        not xi.data.statusEffect.isTargetImmune(target, xi.effect.STUN, xi.element.THUNDER) and
        not xi.data.statusEffect.isTargetResistant(player, target, xi.effect.STUN) and
        not xi.data.statusEffect.isEffectNullified(target, xi.effect.STUN, 0)
    then
        local resistanceRate = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, xi.skillRank.A_PLUS, xi.element.THUNDER, xi.mod.INT, xi.effect.STUN, 0)
        if xi.data.statusEffect.isResistRateSuccessfull(xi.effect.STUN, resistanceRate, 0) then
            target:addStatusEffect(xi.effect.STUN, { power = 1, duration = math.random(2, 8) * resistanceRate, origin = player })
        end
    end

    -- Randomize damage
    local randomizer = 1 + (math.random(1, 5) / 100)

    damage = damage * randomizer
    damage = utils.handleStoneskin(target, damage)

    target:takeDamage(damage, player, xi.attackType.PHYSICAL, xi.damageType.BLUNT)
    target:updateEnmityFromDamage(player, damage)
    ability:setMsg(xi.msg.basic.JA_DAMAGE)

    return damage
end
