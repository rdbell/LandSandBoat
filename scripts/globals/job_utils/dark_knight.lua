-----------------------------------
-- Dark Knight Job Utilities
-- Dual-wired pure inject forms (slice 6732 / 0893):
--   oneHourRecast, activeJobLevel, checkArcaneCrest/WeaponBash,
--   arcaneCircle/Crest, bloodWeapon, darkSeal, diabolicEye,
--   netherVoid, scarletDelirium, souleater, weaponBash pure products
-- Parity: internal/darkknight
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.dark_knight = xi.job_utils.dark_knight or {}

-----------------------------------
-- Pure inject pins (internal/darkknight)
-----------------------------------
xi.job_utils.dark_knight.arcaneCircleMainPower       = 15
xi.job_utils.dark_knight.arcaneCircleSubPower        = 5
xi.job_utils.dark_knight.arcaneCircleBaseDuration    = 180
xi.job_utils.dark_knight.arcaneCrestPower            = 20
xi.job_utils.dark_knight.arcaneCrestBaseDuration     = 180
xi.job_utils.dark_knight.bloodWeaponPower            = 1
xi.job_utils.dark_knight.bloodWeaponBaseDuration     = 30
xi.job_utils.dark_knight.consumeManaPower            = 1
xi.job_utils.dark_knight.consumeManaDuration         = 60
xi.job_utils.dark_knight.darkSealMeritOffset         = 10
xi.job_utils.dark_knight.darkSealDuration            = 60
xi.job_utils.dark_knight.diabolicEyeBasePower        = 15
xi.job_utils.dark_knight.diabolicEyeMeritPowerScale  = 5
xi.job_utils.dark_knight.diabolicEyeBaseDuration     = 180
xi.job_utils.dark_knight.lastResortDuration          = 180
xi.job_utils.dark_knight.netherVoidBasePower         = 50
xi.job_utils.dark_knight.netherVoidJPScale           = 2
xi.job_utils.dark_knight.netherVoidDuration          = 60
xi.job_utils.dark_knight.scarletDeliriumBaseDuration = 90
xi.job_utils.dark_knight.soulEnslavementDuration     = 30
xi.job_utils.dark_knight.souleaterPower              = 1
xi.job_utils.dark_knight.souleaterBaseDuration       = 60
xi.job_utils.dark_knight.weaponBashLevelOffset       = 11
xi.job_utils.dark_knight.weaponBashJPScale           = 10
xi.job_utils.dark_knight.oneHourRecastSecondsPerMod  = 60
xi.job_utils.dark_knight.ecosystemArcana             = 3
xi.job_utils.dark_knight.msgCannotOnThatTarg         = 155
xi.job_utils.dark_knight.msgNeeds2HWeapon            = 307

-- Pure one-hour recast (internal/darkknight.OneHourRecast).
xi.job_utils.dark_knight.oneHourRecastFromParams = function(params)
    params = params or {}
    return math.max(0, (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.dark_knight.oneHourRecastSecondsPerMod)
end

-- Pure active job level inject.
xi.job_utils.dark_knight.activeJobLevel = function(mainJob, subJob, job, mainLevel, subLevel)
    if mainJob == job then
        return mainLevel or 0
    end

    if subJob == job then
        return subLevel or 0
    end

    return 0
end

-- Pure Arcane Crest ecosystem gate.
xi.job_utils.dark_knight.checkArcaneCrestFromParams = function(params)
    params = params or {}
    if (params.targetEcosystem or 0) == xi.job_utils.dark_knight.ecosystemArcana then
        return 0
    end

    return xi.job_utils.dark_knight.msgCannotOnThatTarg
end

-- Pure Weapon Bash 2H gate.
xi.job_utils.dark_knight.checkWeaponBashFromParams = function(params)
    params = params or {}
    if params.isTwoHanded then
        return 0
    end

    return xi.job_utils.dark_knight.msgNeeds2HWeapon
end

-- Pure Arcane Circle power/duration.
xi.job_utils.dark_knight.arcaneCirclePowerFromParams = function(params)
    params = params or {}
    local power = xi.job_utils.dark_knight.arcaneCircleSubPower
    if params.mainJobIsDRK then
        power = xi.job_utils.dark_knight.arcaneCircleMainPower
    end

    return power + (params.arcaneCirclePotencyMod or 0)
end

xi.job_utils.dark_knight.arcaneCircleDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.arcaneCircleBaseDuration + (params.arcaneCircleDurationMod or 0)
end

-- Pure Arcane Crest duration.
xi.job_utils.dark_knight.arcaneCrestDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.arcaneCrestBaseDuration + (params.arcaneCrestJP or 0)
end

-- Pure Blood Weapon duration.
xi.job_utils.dark_knight.bloodWeaponDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.bloodWeaponBaseDuration + (params.enhancesBloodWeaponMod or 0)
end

-- Pure Dark Seal power/subPower.
xi.job_utils.dark_knight.darkSealPowerFromParams = function(params)
    params = params or {}
    return (params.darkSealMerit or 0) - xi.job_utils.dark_knight.darkSealMeritOffset
end

xi.job_utils.dark_knight.darkSealSubPowerFromParams = function(params)
    params = params or {}
    return (params.darkSealMerit or 0) * (params.enhancesDarkSealMod or 0) / 10
end

-- Pure Diabolic Eye power/duration.
xi.job_utils.dark_knight.diabolicEyePowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.diabolicEyeBasePower
        + (params.diabolicEyeMerit or 0) * xi.job_utils.dark_knight.diabolicEyeMeritPowerScale
end

xi.job_utils.dark_knight.diabolicEyeDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.diabolicEyeBaseDuration
        + (params.diabolicEyeMerit or 0) * (params.enhancesDiabolicEyeMod or 0)
end

-- Pure Nether Void power.
xi.job_utils.dark_knight.netherVoidPowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.netherVoidBasePower
        + (params.enhancesNetherVoidMod or 0)
        + xi.job_utils.dark_knight.netherVoidJPScale * (params.netherVoidJP or 0)
end

-- Pure Scarlet Delirium duration.
xi.job_utils.dark_knight.scarletDeliriumDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.scarletDeliriumBaseDuration + (params.scarletDeliriumJP or 0)
end

-- Pure Souleater duration/subPower.
xi.job_utils.dark_knight.souleaterDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.dark_knight.souleaterBaseDuration + (params.souleaterJP or 0)
end

xi.job_utils.dark_knight.souleaterSubPowerFromParams = function(params)
    params = params or {}
    return (params.enhancesMutedSoulMod or 0) * (params.mutedSoulMerit or 0) / 10
end

-- Pure Weapon Bash damage.
xi.job_utils.dark_knight.weaponBashDamageFromParams = function(params)
    params = params or {}
    local lvl = params.darkKnightLvl or 0
    return math.floor(
        (lvl + xi.job_utils.dark_knight.weaponBashLevelOffset) / 4
            + (params.weaponBashMod or 0)
            + (params.weaponBashJP or 0) * xi.job_utils.dark_knight.weaponBashJPScale
    )
end

-- Pure Weapon Bash animation table.
xi.job_utils.dark_knight.weaponBashAnimation = function(mainWeaponSkill)
    local animationTable =
    {
        [xi.skill.GREAT_SWORD ] = 201,
        [xi.skill.GREAT_KATANA] = 201,
        [xi.skill.GREAT_AXE   ] = 202,
        [xi.skill.SCYTHE      ] = 202,
        [xi.skill.STAFF       ] = 202,
        [xi.skill.POLEARM     ] = 203,
    }

    return animationTable[mainWeaponSkill or 0] or 0
end

-----------------------------------
-- Ability Check Functions (hosts → pure)
-----------------------------------
xi.job_utils.dark_knight.checkArcaneCrest = function(player, target, ability)
    return xi.job_utils.dark_knight.checkArcaneCrestFromParams({
        targetEcosystem = target:getEcosystem(),
    }), 0
end

xi.job_utils.dark_knight.checkBloodWeapon = function(player, target, ability)
    ability:setRecast(xi.job_utils.dark_knight.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.dark_knight.checkSoulEnslavement = function(player, target, ability)
    ability:setRecast(xi.job_utils.dark_knight.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.dark_knight.checkWeaponBash = function(player, target, ability)
    return xi.job_utils.dark_knight.checkWeaponBashFromParams({
        isTwoHanded = player:isWeaponTwoHanded(),
    }), 0
end

-----------------------------------
-- Ability Use Functions (hosts → pure)
-----------------------------------
xi.job_utils.dark_knight.useArcaneCircle = function(player, target, ability)
    -- TODO:
    -- Create Bonus vs Ecosystem handling
    -- https://www.bg-wiki.com/ffxi/Arcane_Circle
    -- Main (DRK) job gives a unique 15% damage bonus against arcana, 15% damage resistance from arcana, and likely +15% Arcana Killer.
    -- When subbed, gives 5% of these bonuses.
    --
    -- Job Points bonus will need to be handled in the Bonus vs Ecosystem handling system
    -- https://www.bg-wiki.com/ffxi/Job_Points#Dark_Knight
    -- Arcane Circle Effect: Reduces the amount of damage taken from arcana while under the effects of Arcane Circle.
    local power = xi.job_utils.dark_knight.arcaneCirclePowerFromParams({
        mainJobIsDRK            = player:getMainJob() == xi.job.DRK,
        arcaneCirclePotencyMod  = player:getMod(xi.mod.ARCANE_CIRCLE_POTENCY),
    })
    local duration = xi.job_utils.dark_knight.arcaneCircleDurationFromParams({
        arcaneCircleDurationMod = player:getMod(xi.mod.ARCANE_CIRCLE_DURATION),
    })

    target:addStatusEffect(xi.effect.ARCANE_CIRCLE, { power = power, duration = duration, origin = player })

    return xi.effect.ARCANE_CIRCLE
end

xi.job_utils.dark_knight.useArcaneCrest = function(player, target, ability)
    local duration = xi.job_utils.dark_knight.arcaneCrestDurationFromParams({
        arcaneCrestJP = player:getJobPointLevel(xi.jp.ARCANE_CREST_DURATION),
    })

    target:addStatusEffect(xi.effect.ARCANE_CREST, {
        power    = xi.job_utils.dark_knight.arcaneCrestPower,
        duration = duration,
        origin   = player,
    })
end

xi.job_utils.dark_knight.useBloodWeapon = function(player, target, ability)
    local duration = xi.job_utils.dark_knight.bloodWeaponDurationFromParams({
        enhancesBloodWeaponMod = player:getMod(xi.mod.ENHANCES_BLOOD_WEAPON),
    })

    target:addStatusEffect(xi.effect.BLOOD_WEAPON, {
        power    = xi.job_utils.dark_knight.bloodWeaponPower,
        duration = duration,
        origin   = player,
    })

    return xi.effect.BLOOD_WEAPON
end

xi.job_utils.dark_knight.useConsumeMana = function(player, target, ability)
    player:addStatusEffect(xi.effect.CONSUME_MANA, {
        power    = xi.job_utils.dark_knight.consumeManaPower,
        duration = xi.job_utils.dark_knight.consumeManaDuration,
        origin   = player,
    })

    return xi.effect.CONSUME_MANA
end

xi.job_utils.dark_knight.useDarkSeal = function(player, target, ability, action)
    -- Power: Each merit level after the first reduces Dark Magic casting time by -10% (total of -40% bonus).
    -- Sub Power: Enhances Dark Seal effect by increasing duration of Dark Magic by 10% per merit level (total of 50% bonus).
    local merit = player:getMerit(xi.merit.DARK_SEAL)
    local power = xi.job_utils.dark_knight.darkSealPowerFromParams({ darkSealMerit = merit })
    local subPower = xi.job_utils.dark_knight.darkSealSubPowerFromParams({
        darkSealMerit        = merit,
        enhancesDarkSealMod  = player:getMod(xi.mod.ENHANCES_DARK_SEAL),
    })

    player:addStatusEffect(xi.effect.DARK_SEAL, {
        power    = power,
        duration = xi.job_utils.dark_knight.darkSealDuration,
        origin   = player,
        subPower = subPower,
    })

    return xi.effect.DARK_SEAL
end

xi.job_utils.dark_knight.useDiabolicEye = function(player, target, ability, action)
    local merit = player:getMerit(xi.merit.DIABOLIC_EYE)
    local power = xi.job_utils.dark_knight.diabolicEyePowerFromParams({ diabolicEyeMerit = merit })
    local duration = xi.job_utils.dark_knight.diabolicEyeDurationFromParams({
        diabolicEyeMerit         = merit,
        enhancesDiabolicEyeMod   = player:getMod(xi.mod.ENHANCES_DIABOLIC_EYE),
    })

    player:addStatusEffect(xi.effect.DIABOLIC_EYE, { power = power, duration = duration, origin = player })

    return xi.effect.DIABOLIC_EYE
end

xi.job_utils.dark_knight.useLastResort = function(player, target, ability)
    player:addStatusEffect(xi.effect.LAST_RESORT, {
        duration = xi.job_utils.dark_knight.lastResortDuration,
        origin   = player,
    })

    return xi.effect.LAST_RESORT
end

xi.job_utils.dark_knight.useNetherVoid = function(player, target, ability)
    local power = xi.job_utils.dark_knight.netherVoidPowerFromParams({
        enhancesNetherVoidMod = player:getMod(xi.mod.ENHANCES_NETHER_VOID),
        netherVoidJP          = player:getJobPointLevel(xi.jp.NETHER_VOID_EFFECT),
    })

    player:addStatusEffect(xi.effect.NETHER_VOID, {
        power    = power,
        duration = xi.job_utils.dark_knight.netherVoidDuration,
        origin   = player,
    })

    return xi.effect.NETHER_VOID
end

xi.job_utils.dark_knight.useScarletDelirium = function(player, target, ability)
    local duration = xi.job_utils.dark_knight.scarletDeliriumDurationFromParams({
        scarletDeliriumJP = player:getJobPointLevel(xi.jp.SCARLET_DELIRIUM_DURATION),
    })

    player:addStatusEffect(xi.effect.SCARLET_DELIRIUM, { duration = duration, origin = player })

    return xi.effect.SCARLET_DELIRIUM
end

xi.job_utils.dark_knight.useSoulEnslavement = function(player, target, ability)
    player:addStatusEffect(xi.effect.SOUL_ENSLAVEMENT, {
        duration = xi.job_utils.dark_knight.soulEnslavementDuration,
        origin   = player,
    })

    return xi.effect.SOUL_ENSLAVEMENT
end

xi.job_utils.dark_knight.useSouleater = function(player, target, ability)
    local duration = xi.job_utils.dark_knight.souleaterDurationFromParams({
        souleaterJP = target:getJobPointLevel(xi.jp.SOULEATER_DURATION),
    })
    local subPower = xi.job_utils.dark_knight.souleaterSubPowerFromParams({
        enhancesMutedSoulMod = target:getMod(xi.mod.ENHANCES_MUTED_SOUL),
        mutedSoulMerit       = target:getMerit(xi.merit.MUTED_SOUL),
    })

    player:addStatusEffect(xi.effect.SOULEATER, {
        power    = xi.job_utils.dark_knight.souleaterPower,
        duration = duration,
        origin   = player,
        subPower = subPower,
    })

    return xi.effect.SOULEATER
end

xi.job_utils.dark_knight.useWeaponBash = function(player, target, ability, action)
    -- Damage
    local darkKnightLvl = utils.getActiveJobLevel(player, xi.job.DRK)
    local damage = xi.job_utils.dark_knight.weaponBashDamageFromParams({
        darkKnightLvl = darkKnightLvl,
        weaponBashMod = player:getMod(xi.mod.WEAPON_BASH),
        weaponBashJP  = target:getJobPointLevel(xi.jp.WEAPON_BASH_EFFECT),
    })
    target:takeDamage(damage, player, xi.attackType.PHYSICAL, xi.damageType.BLUNT)
    target:updateEnmityFromDamage(player, damage)

    -- Stun.
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

    -- Animation.
    local animation = xi.job_utils.dark_knight.weaponBashAnimation(player:getWeaponSkillType(xi.slot.MAIN))
    action:setAnimation(target:getID(), animation)

    return damage
end
