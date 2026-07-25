-----------------------------------
-- Monk Job Utilities
-- Dual-wired pure inject forms (slice 6733 / 0896):
--   boostPower, chakra max/recovery/clears, invigorate, chiBlast,
--   stance powers, oneHourRecast, activeJobLevel
-- Parity: internal/monk
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.monk = xi.job_utils.monk or {}

-----------------------------------
-- Pure inject pins (internal/monk)
-----------------------------------
xi.job_utils.monk.boostBasePower              = 12.5
xi.job_utils.monk.boostEffectScale            = 0.10
xi.job_utils.monk.boostDuration               = 180
xi.job_utils.monk.chakraVITScale              = 2
xi.job_utils.monk.chakraLevelHPFactor         = 0.2 / 100
xi.job_utils.monk.invigorateRegenPower        = 10
xi.job_utils.monk.invigorateRegenTier         = 1
xi.job_utils.monk.chiBlastPenancePower        = 25
xi.job_utils.monk.chiBlastBoostPowerDiv       = 100
xi.job_utils.monk.chiBlastBoostMult           = 4
xi.job_utils.monk.chiBlastBaseFraction        = 0.5
xi.job_utils.monk.counterstanceBasePower      = 45
xi.job_utils.monk.counterstanceDuration       = 300
xi.job_utils.monk.dodgeDuration               = 30
xi.job_utils.monk.focusDuration               = 30
xi.job_utils.monk.footworkBasePower           = 20
xi.job_utils.monk.footworkBaseSubPower        = 25
xi.job_utils.monk.footworkDuration            = 60
xi.job_utils.monk.formlessStrikesPower        = 1
xi.job_utils.monk.formlessStrikesDuration     = 180
xi.job_utils.monk.hundredFistsPower           = 1
xi.job_utils.monk.hundredFistsDuration        = 45
xi.job_utils.monk.impetusPower                = 0
xi.job_utils.monk.impetusDuration             = 180
xi.job_utils.monk.innerStrengthPower          = 2
xi.job_utils.monk.innerStrengthDuration       = 30
xi.job_utils.monk.mantraDuration              = 180
xi.job_utils.monk.perfectCounterPower         = 2
xi.job_utils.monk.perfectCounterDuration      = 30
xi.job_utils.monk.oneHourRecastSecondsPerMod  = 60
xi.job_utils.monk.chakraBitParalysis          = 1
xi.job_utils.monk.chakraBitDisease            = 2
xi.job_utils.monk.chakraBitPlague             = 4

local chakraStatusEffects =
{
    POISON       = 0, -- Removed by default
    BLINDNESS    = 0, -- Removed by default
    PARALYSIS    = 1,
    DISEASE      = 2,
    PLAGUE       = 4,
}

-- Pure Boost power (internal/monk.BoostPower).
xi.job_utils.monk.boostPowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.monk.boostBasePower
        + xi.job_utils.monk.boostEffectScale * (params.boostEffectMod or 0)
end

xi.job_utils.monk.boostStackedPower = function(existingPower, addPower)
    return (existingPower or 0) + (addPower or 0)
end

-- Pure Chakra max recovery (internal/monk.ChakraMaxRecovery).
-- params: monkLevel, vit, maxHP, chakraMultMod, chakraJP
xi.job_utils.monk.chakraMaxRecoveryFromParams = function(params)
    params = params or {}
    local monkLevel = params.monkLevel or 0
    local vit       = params.vit or 0
    local maxHP     = params.maxHP or 0
    local multMod   = params.chakraMultMod or 0
    local jp        = params.chakraJP or 0

    local hpModifier = ((monkLevel + 1) * xi.job_utils.monk.chakraLevelHPFactor) * maxHP
    local chakraMultiplier = 1 + multMod / 100
    return (vit * xi.job_utils.monk.chakraVITScale + hpModifier) * chakraMultiplier + jp
end

-- Pure Chakra recovery clamp to missing HP.
xi.job_utils.monk.chakraRecoveryFromParams = function(params)
    params = params or {}
    local currentHP   = params.currentHP or 0
    local maxHP       = params.maxHP or 0
    local maxRecovery = params.maxRecovery or 0
    local deficit     = maxHP - currentHP
    if deficit < 0 then
        deficit = 0
    end

    if maxRecovery < deficit then
        return maxRecovery
    end

    return deficit
end

-- Pure Chakra status-clear bitmask evaluation.
-- returns: { poison, blindness, paralysis, disease, plague }
xi.job_utils.monk.evaluateChakraClears = function(chakraRemovalMod)
    chakraRemovalMod = chakraRemovalMod or 0
    return {
        poison    = true,
        blindness = true,
        paralysis = bit.band(chakraRemovalMod, xi.job_utils.monk.chakraBitParalysis) == xi.job_utils.monk.chakraBitParalysis,
        disease   = bit.band(chakraRemovalMod, xi.job_utils.monk.chakraBitDisease) == xi.job_utils.monk.chakraBitDisease,
        plague    = bit.band(chakraRemovalMod, xi.job_utils.monk.chakraBitPlague) == xi.job_utils.monk.chakraBitPlague,
    }
end

xi.job_utils.monk.invigorateApplies = function(invigorateMerit)
    return (invigorateMerit or 0) > 0
end

xi.job_utils.monk.invigorateDuration = function(invigorateMerit)
    return invigorateMerit or 0
end

-- Pure Chi Blast boost multiplier.
xi.job_utils.monk.chiBlastBoostMultiplierFromParams = function(params)
    params = params or {}
    if not params.hasBoost then
        return 1.0
    end

    return ((params.boostPower or 0) / xi.job_utils.monk.chiBlastBoostPowerDiv)
        * xi.job_utils.monk.chiBlastBoostMult
end

-- Pure Chi Blast damage before ability.adjustDamage.
-- params: mnd, rollUnit (0..1), multiplier
xi.job_utils.monk.chiBlastDamageFromParams = function(params)
    params = params or {}
    local mnd        = params.mnd or 0
    local rollUnit   = params.rollUnit or 0
    local multiplier = params.multiplier or 1
    local base = math.floor(mnd * (xi.job_utils.monk.chiBlastBaseFraction + rollUnit / 2))
    return base * multiplier
end

xi.job_utils.monk.penanceApplies = function(penanceMerit)
    return (penanceMerit or 0) > 0
end

xi.job_utils.monk.penanceDuration = function(penanceMerit)
    return penanceMerit or 0
end

xi.job_utils.monk.counterstancePowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.monk.counterstanceBasePower + (params.counterstanceEffectMod or 0)
end

xi.job_utils.monk.dodgePowerFromParams = function(params)
    params = params or {}
    return (params.dodgeEffectMod or 0) + (params.dodgeJP or 0)
end

xi.job_utils.monk.focusPowerFromParams = function(params)
    params = params or {}
    return (params.focusEffectMod or 0) + (params.focusJP or 0)
end

xi.job_utils.monk.footworkPowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.monk.footworkBasePower + (params.weaponDmg or 0)
end

xi.job_utils.monk.footworkSubPowerFromParams = function(params)
    params = params or {}
    return xi.job_utils.monk.footworkBaseSubPower + (params.footworkAttBonusMod or 0)
end

xi.job_utils.monk.impetusSubPowerFromParams = function(params)
    params = params or {}
    return params.augmentsImpetusMod or 0
end

xi.job_utils.monk.mantraPowerFromParams = function(params)
    params = params or {}
    return params.mantraMerit or 0
end

xi.job_utils.monk.oneHourRecastFromParams = function(params)
    params = params or {}
    return math.max(0, (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.monk.oneHourRecastSecondsPerMod)
end

xi.job_utils.monk.activeJobLevel = function(mainJob, subJob, job, mainLevel, subLevel)
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
xi.job_utils.monk.checkHundredFists = function(player, target, ability)
    ability:setRecast(xi.job_utils.monk.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.monk.checkInnerStrength = function(player, target, ability)
    ability:setRecast(xi.job_utils.monk.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

-----------------------------------
-- Ability Use Functions (hosts → pure)
-----------------------------------
xi.job_utils.monk.useBoost = function(player, target, ability)
    local power = xi.job_utils.monk.boostPowerFromParams({
        boostEffectMod = player:getMod(xi.mod.BOOST_EFFECT),
    })

    if player:hasStatusEffect(xi.effect.BOOST) then
        local effect = player:getStatusEffect(xi.effect.BOOST)
        local stacked = xi.job_utils.monk.boostStackedPower(effect:getPower(), power)

        effect:setPower(stacked) -- Store updated power in boost for zoning
        effect:addMod(xi.mod.ATTP, power)
    else
        player:addStatusEffect(xi.effect.BOOST, {
            power    = power,
            duration = xi.job_utils.monk.boostDuration,
            origin   = player,
        })
    end
end

-- TODO: add Melee Gloves +2 aug
xi.job_utils.monk.useChakra = function(player, target, ability)
    local chakraRemoval = player:getMod(xi.mod.CHAKRA_REMOVAL)
    local clears = xi.job_utils.monk.evaluateChakraClears(chakraRemoval)

    -- Always-clear statuses (Lua value 0) + optional bit-gated statuses.
    if clears.poison then
        player:delStatusEffect(xi.effect.POISON)
    end

    if clears.blindness then
        player:delStatusEffect(xi.effect.BLINDNESS)
    end

    if clears.paralysis then
        player:delStatusEffect(xi.effect.PARALYSIS)
    end

    if clears.disease then
        player:delStatusEffect(xi.effect.DISEASE)
    end

    if clears.plague then
        player:delStatusEffect(xi.effect.PLAGUE)
    end

    -- see https://www.bg-wiki.com/ffxi/Chakra
    local monkLevel = utils.getActiveJobLevel(player, xi.job.MNK)
    local maxRecoveryAmount = xi.job_utils.monk.chakraMaxRecoveryFromParams({
        monkLevel     = monkLevel,
        vit           = player:getStat(xi.mod.VIT),
        maxHP         = player:getMaxHP(),
        chakraMultMod = player:getMod(xi.mod.CHAKRA_MULT),
        chakraJP      = target:getJobPointLevel(xi.jp.CHAKRA_EFFECT), -- NOTE: Level is the modified value, so 10 per point spent
    })
    local recoveryAmount = xi.job_utils.monk.chakraRecoveryFromParams({
        currentHP   = player:getHP(),
        maxHP       = player:getMaxHP(),
        maxRecovery = maxRecoveryAmount,
    })

    player:setHP(player:getHP() + recoveryAmount)

    local merits = player:getMerit(xi.merit.INVIGORATE)
    if xi.job_utils.monk.invigorateApplies(merits) then
        if player:hasStatusEffect(xi.effect.REGEN) then
            player:delStatusEffect(xi.effect.REGEN)
        end

        player:addStatusEffect(xi.effect.REGEN, {
            power    = xi.job_utils.monk.invigorateRegenPower,
            duration = xi.job_utils.monk.invigorateDuration(merits),
            origin   = player,
            tier     = xi.job_utils.monk.invigorateRegenTier,
        })
    end

    return recoveryAmount
end

xi.job_utils.monk.useChiBlast = function(player, target, ability)
    local penanceMerits = player:getMerit(xi.merit.PENANCE) -- 20/40/60/80/100
    if xi.job_utils.monk.penanceApplies(penanceMerits) then
        target:delStatusEffectSilent(xi.effect.INHIBIT_TP)
        target:addStatusEffect(xi.effect.INHIBIT_TP, {
            power    = xi.job_utils.monk.chiBlastPenancePower,
            duration = xi.job_utils.monk.penanceDuration(penanceMerits),
            origin   = player,
        })
    end

    local boost = player:getStatusEffect(xi.effect.BOOST)
    local hasBoost = boost ~= nil
    local boostPower = hasBoost and boost:getPower() or 0
    local multiplier = xi.job_utils.monk.chiBlastBoostMultiplierFromParams({
        hasBoost   = hasBoost,
        boostPower = boostPower,
    })

    local dmg = xi.job_utils.monk.chiBlastDamageFromParams({
        mnd        = player:getStat(xi.mod.MND),
        rollUnit   = math.random(),
        multiplier = multiplier,
    })

    dmg = xi.ability.adjustDamage(dmg, player, ability, target, xi.attackType.BREATH, xi.damageType.ELEMENTAL, xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
    target:takeDamage(dmg, player, xi.attackType.BREATH, xi.damageType.ELEMENTAL)
    target:updateClaim(player)
    player:delStatusEffect(xi.effect.BOOST)

    return dmg
end

xi.job_utils.monk.useCounterstance = function(player, target, ability)
    target:delStatusEffect(xi.effect.COUNTERSTANCE)

    local pTable =
    {
        power    = xi.job_utils.monk.counterstancePowerFromParams({
            counterstanceEffectMod = player:getMod(xi.mod.COUNTERSTANCE_EFFECT),
        }),
        duration = xi.job_utils.monk.counterstanceDuration,
        origin   = player,
    }

    target:addStatusEffect(xi.effect.COUNTERSTANCE, pTable)

    return xi.effect.COUNTERSTANCE
end

xi.job_utils.monk.useDodge = function(player, target, ability)
    local pTable =
    {
        power = xi.job_utils.monk.dodgePowerFromParams({
            dodgeEffectMod = target:getMod(xi.mod.DODGE_EFFECT),
            dodgeJP        = target:getJobPointLevel(xi.jp.DODGE_EFFECT),
        }),
        duration = xi.job_utils.monk.dodgeDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.DODGE, pTable)

    return xi.effect.DODGE
end

xi.job_utils.monk.useFocus = function(player, target, ability)
    local pTable =
    {
        power = xi.job_utils.monk.focusPowerFromParams({
            focusEffectMod = target:getMod(xi.mod.FOCUS_EFFECT),
            focusJP        = target:getJobPointLevel(xi.jp.FOCUS_EFFECT),
        }),
        duration = xi.job_utils.monk.focusDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.FOCUS, pTable)

    return xi.effect.FOCUS
end

xi.job_utils.monk.useFootwork = function(player, target, ability)
    local pTable =
    {
        power = xi.job_utils.monk.footworkPowerFromParams({
            weaponDmg = player:getWeaponDmg(),
        }),
        duration = xi.job_utils.monk.footworkDuration,
        subPower = xi.job_utils.monk.footworkSubPowerFromParams({
            footworkAttBonusMod = player:getMod(xi.mod.FOOTWORK_ATT_BONUS),
        }),
        origin = player,
    }

    player:addStatusEffect(xi.effect.FOOTWORK, pTable)

    return xi.effect.FOOTWORK
end

xi.job_utils.monk.useFormlessStrikes = function(player, target, ability)
    local pTable =
    {
        power    = xi.job_utils.monk.formlessStrikesPower,
        duration = xi.job_utils.monk.formlessStrikesDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.FORMLESS_STRIKES, pTable)

    return xi.effect.FORMLESS_STRIKES
end

xi.job_utils.monk.useHundredFists = function(player, target, ability)
    local pTable =
    {
        power    = xi.job_utils.monk.hundredFistsPower,
        duration = xi.job_utils.monk.hundredFistsDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.HUNDRED_FISTS, pTable)

    return xi.effect.HUNDRED_FISTS
end

xi.job_utils.monk.useImpetus = function(player, target, ability)
    local pTable =
    {
        power = xi.job_utils.monk.impetusPower,
        subPower = xi.job_utils.monk.impetusSubPowerFromParams({
            augmentsImpetusMod = player:getMod(xi.mod.AUGMENTS_IMPETUS),
        }),
        duration = xi.job_utils.monk.impetusDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.IMPETUS, pTable)

    return xi.effect.IMPETUS
end

xi.job_utils.monk.useInnerStrength = function(player, target, ability)
    local pTable =
    {
        power    = xi.job_utils.monk.innerStrengthPower,
        duration = xi.job_utils.monk.innerStrengthDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.INNER_STRENGTH, pTable)

    return xi.effect.INNER_STRENGTH
end

xi.job_utils.monk.useMantra = function(player, target, ability)
    target:delStatusEffect(xi.effect.MAX_HP_BOOST) -- TODO: confirm which versions of HP boost mantra can overwrite

    local pTable =
    {
        power = xi.job_utils.monk.mantraPowerFromParams({
            mantraMerit = player:getMerit(xi.merit.MANTRA),
        }),
        duration = xi.job_utils.monk.mantraDuration,
        origin   = player,
    }

    target:addStatusEffect(xi.effect.MAX_HP_BOOST, pTable)

    return xi.effect.MAX_HP_BOOST
end

xi.job_utils.monk.usePerfectCounter = function(player, target, ability)
    local pTable =
    {
        power    = xi.job_utils.monk.perfectCounterPower,
        duration = xi.job_utils.monk.perfectCounterDuration,
        origin   = player,
    }

    player:addStatusEffect(xi.effect.PERFECT_COUNTER, pTable)

    return xi.effect.PERFECT_COUNTER
end
