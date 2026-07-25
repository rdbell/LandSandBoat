-----------------------------------
-- Dragoon Job Utilities
-- Dual-wired pure inject forms (slice 6748 / 0890, 6032, 6034):
--   jump TP/multipliers, ancient circle, spirit surge, angon,
--   steady wing, spirit link, deep breathing, healing/damage breath,
--   wyvern exp level-ups, high jump enmity, one-hour/breath recast
-- Parity: internal/dragoon
-----------------------------------
require('scripts/globals/ability')
require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/jobpoints')
require('scripts/globals/spells/damage_spell')
require('scripts/globals/weaponskills')
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.dragoon = xi.job_utils.dragoon or {}
-----------------------------------


-----------------------------------
-- Pure inject pins (internal/dragoon, slice 6748)
-----------------------------------
xi.job_utils.dragoon.ancientCircleBaseDuration      = 180
xi.job_utils.dragoon.ancientCircleMainPower         = 15
xi.job_utils.dragoon.ancientCircleSubPower          = 5
xi.job_utils.dragoon.spiritSurgeDuration            = 60
xi.job_utils.dragoon.spiritSurgeMaxHPFraction       = 0.25
xi.job_utils.dragoon.angonBaseDuration              = 15
xi.job_utils.dragoon.angonPower                     = 20
xi.job_utils.dragoon.steadyWingMissingHPFraction    = 0.3
xi.job_utils.dragoon.spiritLinkRegenDuration        = 90
xi.job_utils.dragoon.spiritLinkRegenTick            = 3
xi.job_utils.dragoon.spiritLinkDrainMinFrac         = 0.25
xi.job_utils.dragoon.spiritLinkDrainMaxFrac         = 0.35
xi.job_utils.dragoon.wyvernBreathGearCap            = 64
xi.job_utils.dragoon.wyvernExpCap                   = 1000
xi.job_utils.dragoon.wyvernExpPerLevel              = 200
xi.job_utils.dragoon.oneHourRecastSecondsPerMod     = 60
xi.job_utils.dragoon.highJumpEnmityMain             = 50
xi.job_utils.dragoon.highJumpEnmitySub              = 30
xi.job_utils.dragoon.spiritSurgeHighJumpTPMult      = 2
xi.job_utils.dragoon.superJumpEnmityRange           = 75.0
xi.job_utils.dragoon.recastJump                     = 158
xi.job_utils.dragoon.recastHighJump                 = 159
xi.job_utils.dragoon.recastSuperJump                = 160
xi.job_utils.dragoon.recastSpiritJump               = 166
xi.job_utils.dragoon.recastSoulJump                 = 167

-- Pure: JumpTPBonus
xi.job_utils.dragoon.jumpTPBonusFromParams = function(params)
    params = params or {}
    local extra = params.jumpTPBonus or 0
    if params.isSpiritJump then
        extra = extra + (params.spiritJumpBonus or 0)
    end

    return math.floor(extra * (100 + (params.storeTP or 0)) / 100)
end

-- Pure: WyvernSTRBoost
xi.job_utils.dragoon.wyvernSTRBoostFromParams = function(wyvernLevel)
    return 1 + math.floor((wyvernLevel or 0) / 5)
end

-- Pure: SpiritLinkDrainAmount
xi.job_utils.dragoon.spiritLinkDrainAmountFromParams = function(params)
    params = params or {}
    local fraction = params.fraction or 0
    if fraction < xi.job_utils.dragoon.spiritLinkDrainMinFrac then
        fraction = xi.job_utils.dragoon.spiritLinkDrainMinFrac
    end

    if fraction > xi.job_utils.dragoon.spiritLinkDrainMaxFrac then
        fraction = xi.job_utils.dragoon.spiritLinkDrainMaxFrac
    end

    return math.floor((params.playerHP or 0) * fraction)
end

-- Pure: OneHourRecast
xi.job_utils.dragoon.oneHourRecastFromParams = function(params)
    params = params or {}
    local r = (params.baseRecast or 0)
        - (params.oneHourMod or 0) * xi.job_utils.dragoon.oneHourRecastSecondsPerMod
    if r < 0 then
        return 0
    end

    return r
end

-- Pure: JumpATTMultiplier
xi.job_utils.dragoon.jumpATTMultiplierFromParams = function(jumpATTBonus)
    return ((jumpATTBonus or 0) + 100) / 100
end

-- Pure: JumpFTPFromVIT
xi.job_utils.dragoon.jumpFTPFromVITFromParams = function(vit)
    return 1 + (vit or 0) / 256
end

-- Pure: SpiritJump / SoulJump multipliers — returns atk, tp, forceCrit
xi.job_utils.dragoon.spiritJumpMultipliersFromParams = function(params)
    params = params or {}
    local atk = xi.job_utils.dragoon.jumpATTMultiplierFromParams(params.jumpATTBonus)
        + (params.soulSpiritATTBonus or 0) / 100
    local tp = 1
    local force = false
    if params.hasWyvern then
        atk = atk + 0.25
        tp = 2
        force = true
    end

    return atk, tp, force
end

xi.job_utils.dragoon.soulJumpMultipliersFromParams = function(params)
    params = params or {}
    local atk = xi.job_utils.dragoon.jumpATTMultiplierFromParams(params.jumpATTBonus)
        + (params.soulSpiritATTBonus or 0) / 100
    local tp = 1
    local force = false
    if params.hasWyvern then
        atk = atk + 0.5
        tp = 3
        force = true
    end

    return atk, tp, force
end

-- Pure: Ancient Circle
xi.job_utils.dragoon.ancientCircleDurationFromParams = function(durationMod)
    return xi.job_utils.dragoon.ancientCircleBaseDuration + (durationMod or 0)
end

xi.job_utils.dragoon.ancientCirclePowerFromParams = function(params)
    params = params or {}
    local power = xi.job_utils.dragoon.ancientCircleSubPower
    if params.mainJobIsDRG then
        power = xi.job_utils.dragoon.ancientCircleMainPower + (params.ancientCircleJP or 0)
    end

    return power + (params.potencyMod or 0)
end

-- Pure: Spirit Surge / Angon / Steady Wing / Spirit Link
xi.job_utils.dragoon.spiritSurgeMaxHPBoostFromParams = function(wyvernMaxHP)
    return (wyvernMaxHP or 0) * xi.job_utils.dragoon.spiritSurgeMaxHPFraction
end

xi.job_utils.dragoon.angonDurationFromParams = function(angonMerit)
    return xi.job_utils.dragoon.angonBaseDuration + (angonMerit or 0)
end

xi.job_utils.dragoon.steadyWingStoneskinPowerFromParams = function(params)
    params = params or {}
    return (params.maxHP or 0) * xi.job_utils.dragoon.steadyWingMissingHPFraction
        + ((params.maxHP or 0) - (params.currentHP or 0))
end

xi.job_utils.dragoon.spiritLinkRegenPowerFromParams = function(mainLevel)
    return math.floor((mainLevel or 0) / 3)
end

xi.job_utils.dragoon.spiritLinkTPShareFromParams = function(wyvernTP)
    return (wyvernTP or 0) / 2
end

-- Pure: DeepBreathingBonus
xi.job_utils.dragoon.deepBreathingBonusFromParams = function(params)
    params = params or {}
    if not params.hasEffect then
        return 0
    end

    local merits = params.deepBreathingMerits or 0
    if params.isHealing then
        local bonus = 37.5 + 12.5 * merits
        if params.enhanceDB then
            bonus = bonus + merits * 5
        end

        return bonus
    end

    local bonus = 0.75 + 0.25 * merits
    if params.enhanceDB then
        bonus = bonus + merits * 0.1
    end

    return bonus
end

-- Pure: Healing breath table
xi.job_utils.dragoon.healingBreathTable =
{
    -- ability IDs: 640,641,642,639 = I,II,III,IV in ability.h mapping
    -- use skill:getID() keys when available
}

-- Pure: HealingBreathTier
xi.job_utils.dragoon.healingBreathTierFromParams = function(mainLevel)
    mainLevel = mainLevel or 0
    if mainLevel >= 80 then
        return xi.jobAbility and xi.jobAbility.HEALING_BREATH_IV or 639
    elseif mainLevel >= 40 then
        return xi.jobAbility and xi.jobAbility.HEALING_BREATH_III or 642
    elseif mainLevel >= 20 then
        return xi.jobAbility and xi.jobAbility.HEALING_BREATH_II or 641
    end

    return xi.jobAbility and xi.jobAbility.HEALING_BREATH or 640
end

-- Pure: HealingBreathCurePower
xi.job_utils.dragoon.healingBreathCurePowerFromParams = function(params)
    params = params or {}
    local gear = params.gear or 0
    if gear > xi.job_utils.dragoon.wyvernBreathGearCap then
        gear = xi.job_utils.dragoon.wyvernBreathGearCap
    end

    local multiplier = ((params.baseMult or 0) + gear + math.floor(params.deepMult or 0)) / 256
    return math.floor((params.wyvernMaxHP or 0) * multiplier)
        + (params.base or 0)
        + (params.jobPointBonus or 0) * (params.breathAugmentsBonus or 1)
end

-- Pure: DamageBreathBase
xi.job_utils.dragoon.damageBreathBaseFromParams = function(params)
    params = params or {}
    local gear = params.gear or 0
    if gear > xi.job_utils.dragoon.wyvernBreathGearCap then
        gear = xi.job_utils.dragoon.wyvernBreathGearCap
    end

    local gearMult = 1.0 + gear / 256
    local base = math.floor((params.wyvernHP or 0) / 6 + 15 + (params.jobPointBonus or 0))
    return base * gearMult * (1.0 + (params.breathAugments or 0) + (params.deepMult or 0))
end

xi.job_utils.dragoon.wyvernBreathJPBonusFromParams = function(wyvernBreathJP)
    return (wyvernBreathJP or 0) * 10
end

xi.job_utils.dragoon.breathAugmentsBonusFromParams = function(uncapped)
    return 1 + (uncapped or 0) / 100
end

xi.job_utils.dragoon.damageBreathAugmentsFromParams = function(uncapped)
    return (uncapped or 0) / 100
end

-- Pure: BreathAbilityByResRank — resRanks length 6: fire,ice,wind,earth,thunder,water
xi.job_utils.dragoon.breathAbilityByResRankFromParams = function(resRanks)
    resRanks = resRanks or {}
    local breaths =
    {
        xi.jobAbility and xi.jobAbility.FLAME_BREATH or 646,
        xi.jobAbility and xi.jobAbility.FROST_BREATH or 647,
        xi.jobAbility and xi.jobAbility.GUST_BREATH or 648,
        xi.jobAbility and xi.jobAbility.SAND_BREATH or 649,
        xi.jobAbility and xi.jobAbility.LIGHTNING_BREATH or 650,
        xi.jobAbility and xi.jobAbility.HYDRO_BREATH or 651,
    }
    if #resRanks == 0 then
        return breaths[1]
    end

    local lowest = 11
    local out = breaths[1]
    for i, b in ipairs(breaths) do
        local rank = resRanks[i]
        if rank == nil then
            break
        end

        if rank < lowest then
            lowest = rank
            out = b
        end
    end

    return out
end

-- Pure: WyvernLevelUps — returns levelUps, newExp
xi.job_utils.dragoon.wyvernLevelUpsFromParams = function(params)
    params = params or {}
    local prevExp = params.prevExp or 0
    local exp = params.exp or 0
    if prevExp >= xi.job_utils.dragoon.wyvernExpCap then
        return 0, prevExp
    end

    local current = exp
    if prevExp + current > xi.job_utils.dragoon.wyvernExpCap then
        current = xi.job_utils.dragoon.wyvernExpCap - prevExp
    end

    local levelUps = math.floor((prevExp + current) / xi.job_utils.dragoon.wyvernExpPerLevel)
        - math.floor(prevExp / xi.job_utils.dragoon.wyvernExpPerLevel)
    return levelUps, prevExp + current
end

xi.job_utils.dragoon.breathRecastFromParams = function(params)
    params = params or {}
    local r = (params.baseRecast or 0) - (params.dragoonBreathRecastMod or 0)
    if r < 0 then
        return 0
    end

    return r
end

xi.job_utils.dragoon.highJumpEnmityShedFromParams = function(params)
    params = params or {}
    local base = xi.job_utils.dragoon.highJumpEnmitySub
    if params.mainJobIsDRG then
        base = xi.job_utils.dragoon.highJumpEnmityMain
    end

    return base + (params.highJumpEnmityReduction or 0)
end

xi.job_utils.dragoon.spiritSurgeHighJumpTPRemoveFromParams = function(damage)
    return (damage or 0) * xi.job_utils.dragoon.spiritSurgeHighJumpTPMult
end

xi.job_utils.dragoon.superJumpInRangeFromParams = function(distance)
    return (distance or 0) <= xi.job_utils.dragoon.superJumpEnmityRange
end

xi.job_utils.dragoon.isSpiritOrSoulJumpFromParams = function(abilityID)
    if xi.jobAbility then
        return abilityID == xi.jobAbility.SPIRIT_JUMP or abilityID == xi.jobAbility.SOUL_JUMP
    end

    return abilityID == 260 or abilityID == 293
end

-- Returns a table of WS Parameters common to all damage-dealing jumps
local function getJumpWSParams(player, atkMultiplier, tpMultiplier, forceCrit)
    local params =
    {
        numHits = 1,
        ftpMod  = { 1.0, 1.0, 1.0 },

        -- NOTE: critVaries exists without values since while no modifier, it can crit.
        critVaries = { 0.0, 0.0, 0.0 },
        atkVaries  = { atkMultiplier, atkMultiplier, atkMultiplier },

        bonusTP        = 0,
        targetTPMult   = 0,
        attackerTPMult = tpMultiplier,
        hitsHigh       = true,
        isJump         = true,
    }

    if player:getMod(xi.mod.FORCE_JUMP_CRIT) > 0 or forceCrit then
        params.critVaries = { 1.0, 1.0, 1.0 }
    end

    return params
end

local function getWyvern(player)
    local wyvern = player:getPet()

    if wyvern and wyvern:getPetID() == xi.petId.WYVERN then
        return wyvern
    end

    return nil
end

local function hasWyvern(player)
    return getWyvern(player) and true or false
end

-- Generic Function for damage-based Jumps
-- TODO: implement Fly High attack +5 job points
local function performWSJump(player, target, action, params, abilityID)
    local taChar = player:getTrickAttackChar(target)
    local damage, criticalHit, tpHits, extraHits = xi.weaponskills.doPhysicalWeaponskill(player, target, 0, params, 1000, action, true, taChar)
    local totalHits  = tpHits + extraHits

    if totalHits > 0 then
        if
            abilityID == xi.jobAbility.SOUL_JUMP or
            abilityID == xi.jobAbility.SPIRIT_JUMP
        then
            action:info(target:getID(), 4) -- Special info flag for these abilities.
        end

        -- TODO: process additional effects such as Delphinius, Pteroslaver Mail +2/3, Hebo's Spear, enspells, other weapon built-in add effects

        action:recordDamage(target, xi.attackType.PHYSICAL, damage, criticalHit)
        action:messageID(target:getID(), xi.msg.basic.USES_JA_TAKE_DAMAGE)
    else
        action:messageID(target:getID(), xi.msg.basic.JA_MISS_2)
    end

    -- Jumps add JUMP_TP_BONUS regardless of 0 dmg or miss and is affected by Store TP but not the target's subtle blow
    player:addTP(xi.job_utils.dragoon.jumpTPBonusFromParams({
        jumpTPBonus     = player:getMod(xi.mod.JUMP_TP_BONUS),
        spiritJumpBonus = player:getMod(xi.mod.JUMP_SPIRIT_TP_BONUS),
        storeTP         = player:getMod(xi.mod.STORETP),
        isSpiritJump    = abilityID == xi.jobAbility.SPIRIT_JUMP,
    }))

    -- https://www.bg-wiki.com/ffxi/Fly_High_(Ability)
    if player:hasStatusEffect(xi.effect.FLY_HIGH) then
        local flyHighJumpRecast = 10
        action:setRecast(flyHighJumpRecast)
    end

    return damage, totalHits
end

xi.job_utils.dragoon.cutEmpathyEffectTable = function(validEffects, i, maxCount)
    local delindex = 1

    while maxCount < i do
        delindex = math.random(1, i)

        while validEffects[delindex + 1] ~= nil do
            validEffects[delindex] = validEffects[delindex + 1]
            delindex               = delindex + 1
        end

        validEffects[delindex + 1] = nil -- could be in the above loop, but unsure if Lua allows copying of nil?

        i = i - 1
    end

    return validEffects
end

-- Ability Check Functions
-- Note: This does not include Always-Allow abilitys (return 0, 0 by default)
xi.job_utils.dragoon.abilityCheckRequiresPet = function(player, target, ability, checkActionable)
    if not hasWyvern(player) then
        return xi.msg.basic.REQUIRES_A_PET, 0
    else
        if checkActionable and not player:getPet():canUseAbilities() then
            return xi.msg.basic.PET_CANNOT_DO_ACTION, 0
        end

        if ability:getID() == xi.jobAbility.SPIRIT_SURGE then
            ability:setRecast(xi.job_utils.dragoon.oneHourRecastFromParams({
                baseRecast = ability:getRecast(),
                oneHourMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
            }))
        end

        return 0, 0
    end
end

xi.job_utils.dragoon.abilityCheckCallWyvern = function(player, target, ability)
    if player:getPet() ~= nil then
        return xi.msg.basic.ALREADY_HAS_A_PET, 0
    elseif player:hasStatusEffect(xi.effect.SPIRIT_SURGE) then
        return xi.msg.basic.UNABLE_TO_USE_JA, 0
    elseif not player:canUseMisc(xi.zoneMisc.PET) then
        return xi.msg.basic.CANT_BE_USED_IN_AREA, 0
    else
        return 0, 0
    end
end

xi.job_utils.dragoon.abilityCheckSpiritLink = function(player, target, ability)
    local wyvern = player:getPet()

    if not hasWyvern(player) then
        return xi.msg.basic.REQUIRES_A_PET, 0
    else
        if
            wyvern:getHP() == wyvern:getMaxHP() and
            player:getMerit(xi.merit.EMPATHY) == 0
        then
            return xi.msg.basic.UNABLE_TO_USE_JA, 0
        else
            return 0, 0
        end
    end
end

xi.job_utils.dragoon.abilityCheckDeepBreathing = function(player, target, ability)
    if player:getPet() == nil then
        return xi.msg.basic.REQUIRES_A_PET, 0
    elseif not hasWyvern(player) then
        return xi.msg.basic.NO_EFFECT_ON_PET, 0
    else
        return 0, 0
    end
end

xi.job_utils.dragoon.abilityCheckAngon = function(player, target, ability)
    local id = player:getEquipID(xi.slot.AMMO)

    if id == xi.item.ANGON then
        return 0, 0
    else
        return xi.msg.basic.CANNOT_PERFORM, 0
    end
end

xi.job_utils.dragoon.useSpiritSurge = function(player, target, ability)
    local wyvern   = player:getPet()
    local petTP    = wyvern:getTP()
    local petHP    = wyvern:getHP()
    local duration = xi.job_utils.dragoon.spiritSurgeDuration

    -- Spirit Surge increases dragoon's MAX HP increases by 25% of wyvern MaxHP
    -- bg wiki says 25% ffxiclopedia says 15%, going with 25 for now
    local maxHPBoost = xi.job_utils.dragoon.spiritSurgeMaxHPBoostFromParams(target:getPet():getMaxHP())

    -- Dragoon gets all of wyverns TP when using Spirit Surge
    target:addTP(petTP)
    wyvern:delTP(petTP)

    -- Spirit Surge increases dragoon's Strength
    local strBoost = xi.job_utils.dragoon.wyvernSTRBoostFromParams(wyvern:getMainLvl())

    target:despawnPet()

    -- All Jump recast times are reset, but not Spirit/Soul jump
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastJump)
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastHighJump)
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastSuperJump)

    target:addStatusEffect(xi.effect.SPIRIT_SURGE, {
        power    = maxHPBoost,
        duration = duration,
        origin   = player,
        subPower = strBoost,
    })
    target:addHP(petHP) -- Add in wyvern's remaining HP before the wyvern was despawned
end

xi.job_utils.dragoon.useCallWyvern = function(player, target, ability)
    xi.pet.spawnPet(player, xi.petId.WYVERN)
end

xi.job_utils.dragoon.useAncientCircle = function(player, target, ability)
    local duration = xi.job_utils.dragoon.ancientCircleDurationFromParams(
        player:getMod(xi.mod.ANCIENT_CIRCLE_DURATION)
    )
    local power = xi.job_utils.dragoon.ancientCirclePowerFromParams({
        mainJobIsDRG   = player:getMainJob() == xi.job.DRG,
        ancientCircleJP = player:getJobPointLevel(xi.jp.ANCIENT_CIRCLE_EFFECT),
        potencyMod     = player:getMod(xi.mod.ANCIENT_CIRCLE_POTENCY),
    })

    ability:setMsg(xi.msg.basic.USES_ABILITY_FORTIFIED_DRAGONS)

    target:addStatusEffect(xi.effect.ANCIENT_CIRCLE, {
        power    = power,
        duration = duration,
        origin   = player,
    })

    return xi.effect.ANCIENT_CIRCLE
end

xi.job_utils.dragoon.useJump = function(player, target, ability, action)
    local atkMultiplier = xi.job_utils.dragoon.jumpATTMultiplierFromParams(
        player:getMod(xi.mod.JUMP_ATT_BONUS)
    )
    local params = getJumpWSParams(player, atkMultiplier, 1, false)

    -- Only 'Jump' and not others get the fTP VIT bonus
    local ftp = xi.job_utils.dragoon.jumpFTPFromVITFromParams(player:getStat(xi.mod.VIT))
    params.ftpMod = { ftp, ftp, ftp }

    local damage, totalHits = performWSJump(player, target, action, params, ability:getID())

    -- Under Spirit Surge, Jump also decreases target defense by 20% for 60 seconds
    if
        totalHits > 0 and
        player:hasStatusEffect(xi.effect.SPIRIT_SURGE) and
        not target:hasStatusEffect(xi.effect.DEFENSE_DOWN) -- Does this overwrite itself?
    then
        target:addStatusEffect(xi.effect.DEFENSE_DOWN, { power = 20, duration = 60, origin = player })
    end

    return damage
end

xi.job_utils.dragoon.checkForRemovableEffectsOnSpiritLink = function(player, wyvern)
    -- Removes all DoTs, all at once.
    -- Would this be better as an DoT effect flag?
    -- https://www.ffxiah.com/forum/topic/44396/sigurds-descendants-the-art-of-dragon-slaying/108/#3646578

    -- Confirmed in Brenner:
    wyvern:delStatusEffect(xi.effect.POISON)
    wyvern:delStatusEffect(xi.effect.BIO)
    wyvern:delStatusEffect(xi.effect.DIA)
    wyvern:delStatusEffect(xi.effect.REQUIEM)

    wyvern:delStatusEffect(xi.effect.BURN)
    wyvern:delStatusEffect(xi.effect.FROST)
    wyvern:delStatusEffect(xi.effect.CHOKE)
    wyvern:delStatusEffect(xi.effect.RASP)
    wyvern:delStatusEffect(xi.effect.SHOCK)
    wyvern:delStatusEffect(xi.effect.DROWN)

    -- Player casted doom (Cruel Joke) was removed in brenner 100% of the time
    wyvern:delStatusEffect(xi.effect.DOOM)

    -- If you can use Spirit Link at all, sleep is removed. Empathy merits control use at 100% HP.
    wyvern:delStatusEffect(xi.effect.SLEEP_I)
    wyvern:delStatusEffect(xi.effect.SLEEP_II)
    wyvern:delStatusEffect(xi.effect.LULLABY)

    if player:getMod(xi.mod.ENHANCES_SPIRIT_LINK) > 0 then
        -- https://www.ffxiah.com/forum/topic/44396/sigurds-descendants-the-art-of-dragon-slaying/108/#3646600
        -- Remove 2 erasable effects or effects that can be removed by -na
        local additionalRemovableEffects =
        set{
            xi.effect.BLINDNESS,
            xi.effect.PARALYSIS,
            xi.effect.SILENCE,
            xi.effect.CURSE_I,
            xi.effect.CURSE_II,
            xi.effect.PLAGUE,
            xi.effect.DISEASE,
            xi.effect.PETRIFICATION,
            xi.effect.AMNESIA
        }

        local effects      = wyvern:getStatusEffects()
        local validEffects = {}

        for _, effect in pairs(effects) do
            local id = effect:getEffectType()
            if
                bit.band(effect:getEffectFlags(), xi.effectFlag.ERASABLE) == xi.effectFlag.ERASABLE or
                additionalRemovableEffects[id]
            then
                table.insert(validEffects, id)
            end
        end

        if #validEffects > 0 then
            local removeIndex = math.random(1, #validEffects)

            wyvern:delStatusEffect(validEffects[removeIndex])
            table.remove(validEffects, removeIndex)

            if #validEffects > 0 then
                wyvern:delStatusEffect(validEffects[math.random(1, #validEffects)])
            end
        end
    end
end

xi.job_utils.dragoon.applyEmpathyBonus = function(player, wyvern)
    local empathyTotal = player:getMerit(xi.merit.EMPATHY)

    -- Add wyvern levels to the tune of 200 per empathy merit
    xi.job_utils.dragoon.addWyvernExp(player, 200 * empathyTotal)

    if empathyTotal > 0 then
        ---@type CStatusEffect[]
        local validEffects = {}
        local i            = 0
        local effects      = player:getStatusEffects()
        local copyi        = 0

        for _, effect in pairs(effects) do
            if effect:hasEffectFlag(xi.effectFlag.EMPATHY) then
                validEffects[i + 1] = effect
                i = i + 1
            end
        end

        if i < empathyTotal then
            empathyTotal = i
        elseif i > empathyTotal then
            validEffects = xi.job_utils.dragoon.cutEmpathyEffectTable(validEffects, i, empathyTotal)
        end

        local copyEffect = nil
        while copyi < empathyTotal do
            copyEffect = validEffects[copyi + 1]
            if wyvern:hasStatusEffect(copyEffect:getEffectType()) then
                wyvern:delStatusEffectSilent(copyEffect:getEffectType())
            end

            wyvern:copyStatusEffect(copyEffect)
            copyi = copyi + 1
        end
    end
end

xi.job_utils.dragoon.useSpiritLink = function(player, target, ability, action)
    local wyvern   = player:getPet()
    local playerHP = player:getHP()
    local petTP    = wyvern:getTP()

    xi.job_utils.dragoon.checkForRemovableEffectsOnSpiritLink(player, wyvern)

    -- Empathy: copy status effects and grant wyvern EXP
    xi.job_utils.dragoon.applyEmpathyBonus(player, wyvern)

    local regenAmount = xi.job_utils.dragoon.spiritLinkRegenPowerFromParams(player:getMainLvl())
    wyvern:addStatusEffect(xi.effect.REGEN, {
        power    = regenAmount,
        duration = xi.job_utils.dragoon.spiritLinkRegenDuration,
        origin   = player,
        tick     = xi.job_utils.dragoon.spiritLinkRegenTick,
    }) -- 90 seconds of regen
    local tpShare = xi.job_utils.dragoon.spiritLinkTPShareFromParams(petTP)
    player:addTP(tpShare) -- add half wyvern tp to you
    wyvern:delTP(tpShare) -- remove half tp from wyvern

    -- Calculate drain amount.
    -- TODO: Shouldnt this be floored at some point, so we don't remove 1.5 hp from player health pool and/or stoneskin power?
    local drainamount = 0

    if wyvern:getHP() ~= wyvern:getMaxHP() then
        local fraction = math.random(25, 35) / 100
        drainamount = xi.job_utils.dragoon.spiritLinkDrainAmountFromParams({
            playerHP = playerHP,
            fraction = fraction,
        })
        drainamount = drainamount * (1 - (0.01 * player:getJobPointLevel(xi.jp.SPIRIT_LINK_EFFECT)))
    end

    -- Handle Stoneskin.
    local stoneskinPower = 0

    if player:hasStatusEffect(xi.effect.STONESKIN) then
        stoneskinPower = player:getMod(xi.mod.STONESKIN)

        -- If stoneskin is more powerfull than the amount to be drained.
        if stoneskinPower > drainamount then
            local effect = player:getStatusEffect(xi.effect.STONESKIN)
            effect:setPower(effect:getPower() - drainamount) -- Fixes the status effect so when it ends it uses the new power instead of old.
            player:delMod(xi.mod.STONESKIN, drainamount)     -- Removes the amount from the mod.

        -- If stoneskin is as powerful or less than the amount to be drained.
        else
            player:delStatusEffect(xi.effect.STONESKIN)
        end
    end

    -- Handle master damage and pet healing.
    player:takeDamage(drainamount - stoneskinPower)

    local healPet = drainamount * 2

    if player:getEquipID(xi.slot.HEAD) == xi.item.DRACHEN_ARMET_P1 then
        healPet = healPet + 15
    end

    -- Spirit Link is self target but reports effect on Wyvern.
    action:ID(player:getID(), wyvern:getID())
    return wyvern:addHP(healPet) -- add the hp to wyvern
end

xi.job_utils.dragoon.useHighJump = function(player, target, ability, action)
    local params            = getJumpWSParams(player, 1, 1, false)
    local damage, totalHits = performWSJump(player, target, action, params, ability:getID())

    if target:isMob() then
        local enmityShed = xi.job_utils.dragoon.highJumpEnmityShedFromParams({
            mainJobIsDRG             = player:getMainJob() == xi.job.DRG,
            highJumpEnmityReduction  = player:getMod(xi.mod.HIGH_JUMP_ENMITY_REDUCTION),
        })

        target:lowerEnmity(player, enmityShed) -- reduce total accumulated enmity
    end

    if
        totalHits > 0 and
        player:hasStatusEffect(xi.effect.SPIRIT_SURGE)
    then
        -- Under Spirit Surge, High Jump reduces TP of target
        -- https://www.bg-wiki.com/ffxi/Spirit_Surge
        target:delTP(xi.job_utils.dragoon.spiritSurgeHighJumpTPRemoveFromParams(damage))
    end

    return damage
end

xi.job_utils.dragoon.useSuperJump = function(player, target, ability)
    -- http://wiki.ffo.jp/html/3367.html
    for _, mob in pairs(player:getNotorietyList()) do
        -- TODO: testing shows max range on this is >50' but stops somewhere above this. Need exact number.
        if
            mob:isMob() and
            xi.job_utils.dragoon.superJumpInRangeFromParams(mob:checkDistance(player))
        then
            mob:setCE(player, 1)
            mob:setVE(player, 0)
        end
    end

    ability:setMsg(xi.msg.basic.NONE)

    -- Prevent the player from performing actions while in the air
    player:queue(0, function(playerArg)
        playerArg:untargetableAndUnactionable(5000)
    end)

    -- If the Dragoon's wyvern is out, alive, and engaged, tell it to use Super Climb
    local wyvern = getWyvern(player)
    if
        wyvern ~= nil and
        wyvern:getHP() > 0 and
        wyvern:isEngaged()
    then
        wyvern:usePetAbility(xi.jobAbility.SUPER_CLIMB, wyvern)
    end

    -- Handle Spirit Surge enmity reduction on super jump
    xi.job_utils.dragoon.superJumpSurgeEffect(player, target)
end

xi.job_utils.dragoon.superJumpSurgeEffect = function(player, target)
    if player:hasStatusEffect(xi.effect.SPIRIT_SURGE) then
        local minDistance = 9999
        local closestPartyMember = nil

        -- Find the closest party member
        local party = player:getPartyWithTrusts()
        for _, member in pairs(party) do
            local distance = member:checkDistance(player)
            if
                member:getID() ~= player:getID() and
                not member:isDead() and
                (distance < minDistance or closestPartyMember == nil)
            then
                closestPartyMember = member
                minDistance = distance
            end
        end

        -- TODO: verify conditions for how close the dragoon needs to be to the mob, if at all
        -- It doesn't matter what direction the dragoon is facing http://wiki.ffo.jp/html/3367.html#comment_1
        if
            closestPartyMember and
            closestPartyMember:isBehind(player) and
            (player:checkDistance(target) < closestPartyMember:checkDistance(target)) -- Verify dragoon is closer than the party member that we want to reduce the enmity of
        then
            if target:isMob() then
                target:lowerEnmity(closestPartyMember, 100)
            end
        end
    end
end

-- https://www.bg-wiki.com/ffxi/Angon
xi.job_utils.dragoon.useAngon = function(player, target, ability)
    local duration = xi.job_utils.dragoon.angonDurationFromParams(
        player:getMerit(xi.merit.ANGON)
    ) -- This will return 30 sec at one investment because merit power is 15.

    if not target:addStatusEffect(xi.effect.DEFENSE_DOWN, {
        power    = xi.job_utils.dragoon.angonPower,
        duration = duration,
        origin   = player,
    }) then
        ability:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
    end

    target:updateClaim(player)
    player:removeAmmo(1)

    return xi.effect.DEFENSE_DOWN
end

xi.job_utils.dragoon.useDeepBreathing = function(player, target, ability, action)
    local wyvern = getWyvern(player)

    if wyvern then
        wyvern:addStatusEffect(xi.effect.MAGIC_ATK_BOOST, { duration = 180, origin = player }) -- Message when effect is lost is 'Magic Attack boost wears off.'
    end
end

xi.job_utils.dragoon.useSpiritBond = function(player, target, ability)
    player:addStatusEffect(xi.effect.SPIRIT_BOND, { duration = 180, origin = player })

    return xi.effect.SPIRIT_BOND
end

xi.job_utils.dragoon.useSpiritJump = function(player, target, ability, action)
    -- https://www.bg-wiki.com/ffxi/Spirit_Jump
    local atkMultiplier, tpMultiplier, forceCrit =
        xi.job_utils.dragoon.spiritJumpMultipliersFromParams({
            jumpATTBonus       = player:getMod(xi.mod.JUMP_ATT_BONUS),
            soulSpiritATTBonus = player:getMod(xi.mod.JUMP_SOUL_SPIRIT_ATT_BONUS),
            hasWyvern          = hasWyvern(player),
        })

    local params    = getJumpWSParams(player, atkMultiplier, tpMultiplier, forceCrit)
    local damage, _ = performWSJump(player, target, action, params, ability:getID())

    return damage
end

xi.job_utils.dragoon.useSoulJump = function(player, target, ability, action)
    -- https://www.bg-wiki.com/ffxi/Soul_Jump
    local atkMultiplier, tpMultiplier, forceCrit =
        xi.job_utils.dragoon.soulJumpMultipliersFromParams({
            jumpATTBonus       = player:getMod(xi.mod.JUMP_ATT_BONUS),
            soulSpiritATTBonus = player:getMod(xi.mod.JUMP_SOUL_SPIRIT_ATT_BONUS),
            hasWyvern          = hasWyvern(player),
        })

    local params    = getJumpWSParams(player, atkMultiplier, tpMultiplier, forceCrit)
    local damage, _ = performWSJump(player, target, action, params, ability:getID())

    return damage
end

xi.job_utils.dragoon.useDragonBreaker = function(player, target, ability)
    target:addStatusEffect(xi.effect.DRAGON_BREAKER, { power = 20, duration = 180, origin = player })
end

xi.job_utils.dragoon.useFlyHigh = function(player, target, ability)
    -- All Jump recast times are reset
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastJump)
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastHighJump)
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastSuperJump)
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastSpiritJump)
    target:resetRecast(xi.recast.ABILITY, xi.job_utils.dragoon.recastSoulJump)

    player:addStatusEffect(xi.effect.FLY_HIGH, { duration = 30, origin = player })

    return xi.effect.FLY_HIGH
end

xi.job_utils.dragoon.useSteadyWing = function(player, target, ability, action)
    local wyvern = getWyvern(player)

    -- https://www.bg-wiki.com/ffxi/Steady_Wing
    if wyvern then
        local power = xi.job_utils.dragoon.steadyWingStoneskinPowerFromParams({
            maxHP     = wyvern:getMaxHP(),
            currentHP = wyvern:getHP(),
        })

        if wyvern:addStatusEffect(xi.effect.STONESKIN, { power = power, duration = 300, origin = player }) then
            local effect = wyvern:getStatusEffect(xi.effect.STONESKIN)

            if effect then
                effect:delEffectFlag(xi.effectFlag.DISPELABLE) -- Observed to not be dispelable
                effect:setTier(5) -- Empathy doesn't overwrite this stoneskin wih player casted stoneskin
            end
        end

        -- Steady Wing is self target but reports effect on Wyvern.
        action:ID(player:getID(), wyvern:getID())
    end
end

xi.job_utils.dragoon.getDeepBreathingBonus = function(wyvern, master, isHealing)
    local hadEffect = wyvern:hasStatusEffect(xi.effect.MAGIC_ATK_BOOST)
    local bonus = xi.job_utils.dragoon.deepBreathingBonusFromParams({
        hasEffect           = hadEffect,
        deepBreathingMerits = master:getMerit(xi.merit.DEEP_BREATHING),
        enhanceDB           = master:getMod(xi.mod.ENHANCE_DEEP_BREATHING) > 0,
        isHealing           = isHealing,
    })

    if hadEffect then
        wyvern:delStatusEffect(xi.effect.MAGIC_ATK_BOOST)
    end

    return bonus
end

-- Breath Formula: https://www.bg-wiki.com/ffxi/Wyvern_(Dragoon_Pet)#Healing_Breath
xi.job_utils.dragoon.useHealingBreath = function(wyvern, target, skill, action)
    local healingBreathTable =
    {
        --                                   { base, multiplier }
        [xi.jobAbility.HEALING_BREATH    ] = {  8, 35 },
        [xi.jobAbility.HEALING_BREATH_II ] = { 24, 48 },
        [xi.jobAbility.HEALING_BREATH_III] = { 42, 55 },
        [xi.jobAbility.HEALING_BREATH_IV ] = { 60, 63 },
    }

    local master              = wyvern:getMaster()
    local deepMult            = xi.job_utils.dragoon.getDeepBreathingBonus(wyvern, master, true)
    local jobPointBonus       = xi.job_utils.dragoon.wyvernBreathJPBonusFromParams(
        master:getJobPointLevel(xi.jp.WYVERN_BREATH_EFFECT)
    )
    local breathAugmentsBonus = xi.job_utils.dragoon.breathAugmentsBonusFromParams(
        master:getMod(xi.mod.UNCAPPED_WYVERN_BREATH)
    )
    local gear                = master:getMod(xi.mod.WYVERN_BREATH) -- Master gear that enhances breath
    local base                = healingBreathTable[skill:getID()][1]
    local baseMultiplier      = healingBreathTable[skill:getID()][2]

    local curePower = xi.job_utils.dragoon.healingBreathCurePowerFromParams({
        wyvernMaxHP          = wyvern:getMaxHP(),
        base                 = base,
        baseMult             = baseMultiplier,
        gear                 = gear,
        deepMult             = deepMult,
        jobPointBonus        = jobPointBonus,
        breathAugmentsBonus  = breathAugmentsBonus,
    })
    local totalHPRestored = target:addHP(curePower)

    skill:setMsg(xi.msg.basic.JA_RECOVERS_HP_2)

    -- also cure the Wyvern if Spirit Bond is up
    if master:hasStatusEffect(xi.effect.SPIRIT_BOND) then
        local totalWyvernHPRestored = wyvern:addHP(curePower)

        action:addAdditionalTarget(wyvern:getID())
        action:setAnimation(wyvern:getID(), action:getAnimation(target:getID()))
        action:messageID(wyvern:getID(), xi.msg.basic.SELF_HEAL_SECONDARY)
        action:param(wyvern:getID(), totalWyvernHPRestored)
    end

    if master:getMod(xi.mod.ENHANCES_STRAFE) > 0 then
        wyvern:addTP(master:getMerit(xi.merit.STRAFE_EFFECT) * 50) -- add 50 TP per merit with augmented AF2 legs
    end

    return totalHPRestored
end

-- https://www.bg-wiki.com/ffxi/Wyvern_(Dragoon_Pet)#Elemental_Breath
xi.job_utils.dragoon.useDamageBreath = function(wyvern, target, skill, action, damageType)
    local master                  = wyvern:getMaster()
    local deepBreathingMultiplier = xi.job_utils.dragoon.getDeepBreathingBonus(wyvern, master, false)
    local jobPointBonus           = xi.job_utils.dragoon.wyvernBreathJPBonusFromParams(
        master:getJobPointLevel(xi.jp.WYVERN_BREATH_EFFECT)
    )
    local breathAugmentsBonus     = xi.job_utils.dragoon.damageBreathAugmentsFromParams(
        master:getMod(xi.mod.UNCAPPED_WYVERN_BREATH)
    )
    local gear                    = master:getMod(xi.mod.WYVERN_BREATH) -- Master gear that enhances breath

    local damage = xi.job_utils.dragoon.damageBreathBaseFromParams({
        wyvernHP        = wyvern:getHP(),
        jobPointBonus   = jobPointBonus,
        gear            = gear,
        breathAugments  = breathAugmentsBonus,
        deepMult        = deepBreathingMultiplier,
    })

    -- strafe merits are +10 per merit
    local strafeMeritPower = master:getMerit(xi.merit.STRAFE_EFFECT)
    if master:getMod(xi.mod.ENHANCES_STRAFE) > 0 then
        wyvern:addTP(strafeMeritPower * 5) -- add 50 TP per merit with augmented AF2 legs
    end

    local bonusMacc          = strafeMeritPower + master:getMod(xi.mod.WYVERN_BREATH_MACC)
    local element            = damageType - xi.damageType.ELEMENTAL
    local _, skillchainCount = xi.magicburst.formMagicBurst(target, element)

    -- Breath accuracy is directly affected by a wyvern's current HP, but no data exists.
    local resist              = xi.combat.magicHitRate.calculateResistRate(wyvern, target, 0, 0, 0, element, 0, 0, bonusMacc)
    local sdt                 = xi.combat.damage.magicalElementSDT(target, element)
    local absorb              = xi.spells.damage.calculateAbsorption(target, element, true)
    local nullify             = xi.spells.damage.calculateNullification(target, element, true, true)
    local magicBurst          = 1

    if skillchainCount > 0 then
        magicBurst = xi.spells.damage.calculateIfMagicBurst(target, element, skillchainCount)
    end

    -- It appears that MB breaths don't do more damage based on testing.
    damage = damage * resist * sdt * absorb * nullify

    if damage >= 0 then
        damage = xi.ability.adjustDamage(damage, wyvern, skill, target, xi.attackType.BREATH, damageType, xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        action:recordDamage(target, xi.attackType.BREATH, damage)
        action:messageID(target:getID(), xi.msg.basic.USES_JA_TAKE_DAMAGE)

        if magicBurst > 1 then
            action:messageID(target:getID(), xi.msg.basic.JA_MAGIC_BURST) -- Magic Burst! Target takes X points of damage
        end

        target:takeDamage(damage, wyvern, xi.attackType.BREATH, damageType)
    else
        -- absorb

        -- Capped in 2022 --
        -- retail uses message 121, 'Wyvern uses Frost Breath.\nWyvern recovers <amount> HP.' which is wrong
        -- if SE ever fixes this, it will need to change
        -- skill:setMsg(???)
        -- if magicBurst > 1  then
            -- skill:setMsg(???)
        -- end

        -- Borrow Rune Fencer's behavior for now, including setting the Magic Burst bit.
        -- The bit does not actually change the message.
        action:messageID(target:getID(), xi.msg.basic.JA_RECOVERS_HP)
        if magicBurst > 1  then
            action:modifier(target:getID(), xi.msg.actionModifier.MAGIC_BURST)
        end

        return target:addHP(math.abs(damage))
    end

    return damage
end

-- There is an instance of the wyvern refusing to use breaths on retail, such as against Shinryu.
-- The wyvern will not respond to Smiting Breath, as you are simply unable to use it.
xi.job_utils.dragoon.pickAndUseDamageBreath = function(player, target)
    -- https://www.bg-wiki.com/ffxi/Wyvern_(Dragoon_Pet)#Elemental_Breath
    -- The wyvern simply picks the lowest resistance breath and no longer relies on Drachen Armet et al
    -- if all resistances are equal, Flame Breath is picked first.
    local resRanks =
    {
        target:getMod(xi.mod.FIRE_RES_RANK),
        target:getMod(xi.mod.ICE_RES_RANK),
        target:getMod(xi.mod.WIND_RES_RANK),
        target:getMod(xi.mod.EARTH_RES_RANK),
        target:getMod(xi.mod.THUNDER_RES_RANK),
        target:getMod(xi.mod.WATER_RES_RANK),
    }
    local breathToUse = xi.job_utils.dragoon.breathAbilityByResRankFromParams(resRanks)

    player:getPet():usePetAbility(breathToUse, target)
end

xi.job_utils.dragoon.useRestoringBreath = function(player, ability, action)
    local wyvern          = player:getPet()
    local healingbreath   = xi.job_utils.dragoon.healingBreathTierFromParams(player:getMainLvl())
    local breathHealRange = 14

    local function inBreathRange(target)
        return wyvern:checkDistance(target) <= breathHealRange
    end

    local highestHPDiff = -1
    local target        = nil

    -- Find the target with the most HP diff from max
    local party = player:getPartyWithTrusts()
    for _, member in pairs(party) do
        local maxHPDiff = member:getMaxHP() - member:getHP()
        if
            inBreathRange(member) and
            not member:isDead() and
            (maxHPDiff > highestHPDiff and maxHPDiff > 0) -- Dont pick target if they have full HP
        then
            target = member
            highestHPDiff = maxHPDiff
        end
    end

    if target == nil then -- If no one else found, target master
        target = player
    end

    action:setRecast(xi.job_utils.dragoon.breathRecastFromParams({
        baseRecast              = ability:getRecast(),
        dragoonBreathRecastMod  = player:getMod(xi.mod.DRAGOON_BREATH_RECAST),
    }))

    wyvern:usePetAbility(healingbreath, target)
end

xi.job_utils.dragoon.useSmitingBreath = function(player, target, ability, action)
    action:setRecast(xi.job_utils.dragoon.breathRecastFromParams({
        baseRecast              = ability:getRecast(),
        dragoonBreathRecastMod  = player:getMod(xi.mod.DRAGOON_BREATH_RECAST),
    }))

    xi.job_utils.dragoon.pickAndUseDamageBreath(player, target)
end

xi.job_utils.dragoon.addWyvernExp = function(player, exp)
    local wyvern      = player:getPet()
    local prevExp     = wyvern:getLocalVar('wyvern_exp')
    local numLevelUps, newExp = xi.job_utils.dragoon.wyvernLevelUpsFromParams({
        prevExp = prevExp,
        exp     = exp,
    })

    if numLevelUps > 0 or newExp ~= prevExp then
        if numLevelUps ~= 0 then
            local wyvernAttributeIncreaseEffectJP = player:getJobPointLevel(xi.jp.WYVERN_ATTR_BONUS)
            local wyvernBonusDA = player:getMod(xi.mod.WYVERN_ATTRIBUTE_DA)

            wyvern:addMod(xi.mod.ACC, 6 * numLevelUps)
            wyvern:addMod(xi.mod.HPP, 6 * numLevelUps)
            wyvern:addMod(xi.mod.ATTP, 5 * numLevelUps)

            wyvern:updateHealth()
            wyvern:setHP(wyvern:getMaxHP())

            player:messageBasic(xi.msg.basic.STATUS_INCREASED, 0, 0, wyvern)

            player:addMod(xi.mod.ATT, wyvernAttributeIncreaseEffectJP * numLevelUps)
            player:addMod(xi.mod.DEF, wyvernAttributeIncreaseEffectJP * numLevelUps)
            player:addMod(xi.mod.ATTP, 4 * numLevelUps)
            player:addMod(xi.mod.DEFP, 4 * numLevelUps)
            player:addMod(xi.mod.HASTE_ABILITY, 200 * numLevelUps)
            player:addMod(xi.mod.DOUBLE_ATTACK, wyvernBonusDA * numLevelUps)
            player:addMod(xi.mod.ALL_WSDMG_ALL_HITS, 2 * numLevelUps)
        end

        wyvern:setLocalVar('wyvern_exp', newExp)
        wyvern:setLocalVar('level_Ups', wyvern:getLocalVar('level_Ups') + numLevelUps)
    end

    return numLevelUps
end
