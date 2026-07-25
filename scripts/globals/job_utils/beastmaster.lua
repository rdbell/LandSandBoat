-----------------------------------
-- Beastmaster Job Utilities
-- Dual-wired pure inject forms (slice 6737 / 0886):
--   charm chance/duration, gauge message, jug pet gate, reward heal/regen,
--   stay tick, killer instinct/spur/feral howl, one-hour recast
-- Parity: internal/beastmaster
-----------------------------------
require('scripts/globals/ability')
require('scripts/globals/jobpoints')
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.beastmaster = xi.job_utils.beastmaster or {}

-----------------------------------
-- Helper Tables
-----------------------------------

xi.job_utils.beastmaster.petFoodData =
{
    [xi.item.PET_FOOD_ALPHA_BISCUIT]   = { minHealing =   50, regen =  1, mndMult = 2, mndThreshold = 10 },
    [xi.item.PET_FOOD_BETA_BISCUIT]    = { minHealing =  180, regen =  3, mndMult = 1, mndThreshold = 33 },
    [xi.item.PET_FOOD_GAMMA_BISCUIT]   = { minHealing =  300, regen =  5, mndMult = 1, mndThreshold = 35 }, -- TO BE VERIFIED.
    [xi.item.PET_FOOD_DELTA_BISCUIT]   = { minHealing =  530, regen =  8, mndMult = 2, mndThreshold = 40 }, -- TO BE VERIFIED.
    [xi.item.PET_FOOD_EPSILON_BISCUIT] = { minHealing =  750, regen = 11, mndMult = 2, mndThreshold = 45 },
    [xi.item.PET_FOOD_ZETA_BISCUIT]    = { minHealing =  900, regen = 14, mndMult = 3, mndThreshold = 45 },
    [xi.item.PET_FOOD_ETA_BISCUIT]     = { minHealing = 1200, regen = 17, mndMult = 4, mndThreshold = 50 },
    [xi.item.PET_FOOD_THETA_BISCUIT]   = { minHealing = 1600, regen = 20, mndMult = 4, mndThreshold = 55 },
}


-----------------------------------
-- Pure inject pins (internal/beastmaster, slice 6737 / 0886)
-----------------------------------
xi.job_utils.beastmaster.charmChanceBase              = 50
xi.job_utils.beastmaster.charmChanceCap               = 95
xi.job_utils.beastmaster.charmDurationCHRScale        = 1.25
xi.job_utils.beastmaster.charmDurationCHRAddend       = 150
xi.job_utils.beastmaster.charmTimePerMod              = 0.05
xi.job_utils.beastmaster.rewardRegenDuration          = 180
xi.job_utils.beastmaster.rewardRegenTick              = 3
xi.job_utils.beastmaster.stayTickMax                  = 10
xi.job_utils.beastmaster.stayTickMin                  = 5
xi.job_utils.beastmaster.killerInstinctBaseDuration   = 180
xi.job_utils.beastmaster.killerInstinctMeritUnit      = 10
xi.job_utils.beastmaster.killerInstinctPower          = 10
xi.job_utils.beastmaster.spurBasePower                = 20
xi.job_utils.beastmaster.spurJPAttackPerLevel         = 3
xi.job_utils.beastmaster.spurDuration                 = 90
xi.job_utils.beastmaster.feralHowlBaseDuration        = 10
xi.job_utils.beastmaster.feralHowlMeritUnit           = 5
xi.job_utils.beastmaster.oneHourRecastSecondsPerMod   = 60
xi.job_utils.beastmaster.unleashPower                 = 9
xi.job_utils.beastmaster.unleashDuration              = 60
xi.job_utils.beastmaster.msgCannotCharm               = 210
xi.job_utils.beastmaster.msgVeryDifficultCharm        = 211
xi.job_utils.beastmaster.msgDifficultToCharm          = 212
xi.job_utils.beastmaster.msgMightBeAbleCharm          = 213
xi.job_utils.beastmaster.msgShouldBeAbleCharm         = 214

-- Pure: dLvl charm duration multiplier
xi.job_utils.beastmaster.dLvlCharmMultiplierFromParams = function(dLvl)
    dLvl = dLvl or 0
    if dLvl < -6 then
        return 1 / 24
    end

    if dLvl >= 9 then
        return 6
    end

    return 0.9997336 + 0.3652882 * dLvl + 0.02097742 * dLvl ^ 2
        - 0.004106429 * dLvl ^ 3 + 0.000007231037 * dLvl ^ 4
        + 0.00005102634 * dLvl ^ 5
end

-- Pure: CharmDuration
-- params: charmerCHR, charmerLevel, targetLevel, charmTimeMod
xi.job_utils.beastmaster.charmDurationFromParams = function(params)
    params = params or {}
    local base = math.floor(xi.job_utils.beastmaster.charmDurationCHRScale * (params.charmerCHR or 0)
        + xi.job_utils.beastmaster.charmDurationCHRAddend)
    local dLvl = (params.charmerLevel or 0) - (params.targetLevel or 0)
    local dur = base * xi.job_utils.beastmaster.dLvlCharmMultiplierFromParams(dLvl)
    dur = dur + dur * ((params.charmTimeMod or 0) * xi.job_utils.beastmaster.charmTimePerMod)
    return math.floor(dur)
end

-- Pure: ValidJugPetID
-- params: ammoSubSkill, ammoSkill, ammoPresent, ammoReqLevel, playerMainLevel
-- returns: petId or nil
xi.job_utils.beastmaster.validJugPetIDFromParams = function(params)
    params = params or {}
    if
        (params.ammoSkill or 0) ~= 0 or
        not params.ammoPresent or
        (params.playerMainLevel or 0) < (params.ammoReqLevel or 0)
    then
        return nil
    end

    local petId = params.ammoSubSkill or 0
    local sheepFamiliar = 21
    if xi.petId and xi.petId.SHEEP_FAMILIAR then
        sheepFamiliar = xi.petId.SHEEP_FAMILIAR
    end

    if petId >= sheepFamiliar then
        return petId
    end

    return nil
end

-- Pure: CharmChance
-- params: eligible, charmerBSTLevel, targetLevel, charmRes, lightResRank,
--         includeMods, charmChanceMod, charmerCHR, targetCHR
xi.job_utils.beastmaster.charmChanceFromParams = function(params)
    params = params or {}
    if not params.eligible then
        return 0
    end

    local chance = xi.job_utils.beastmaster.charmChanceBase - (params.charmRes or 0)
    local charmerLvl = params.charmerBSTLevel or 0
    local targetLvl  = params.targetLevel or 0

    if charmerLvl < targetLvl then
        local dLvl = targetLvl - charmerLvl
        if targetLvl >= 71 then
            chance = chance - 10 * dLvl
        elseif targetLvl >= 51 then
            chance = chance - 5 * dLvl
        else
            chance = chance - 3 * dLvl
        end
    end

    local rank = params.lightResRank or 0
    if rank <= -3 then
        chance = chance * 1.5
    elseif rank <= -2 then
        chance = chance * 1.4
    elseif rank <= -1 then
        chance = chance * 1.2
    elseif rank <= 0 then
        -- identity
    else
        chance = chance / 2
    end

    if params.includeMods then
        chance = chance + (params.charmChanceMod or 0)
    end

    chance = chance + ((params.charmerCHR or 0) - (params.targetCHR or 0))

    if chance < 0 then
        return 0
    end

    if chance > xi.job_utils.beastmaster.charmChanceCap then
        return xi.job_utils.beastmaster.charmChanceCap
    end

    return chance
end

-- Pure: GaugeMessage
xi.job_utils.beastmaster.gaugeMessageFromParams = function(chance)
    chance = chance or 0
    if chance >= 75 then
        return xi.job_utils.beastmaster.msgShouldBeAbleCharm
    elseif chance >= 50 then
        return xi.job_utils.beastmaster.msgMightBeAbleCharm
    elseif chance >= 25 then
        return xi.job_utils.beastmaster.msgDifficultToCharm
    elseif chance >= 1 then
        return xi.job_utils.beastmaster.msgVeryDifficultCharm
    end

    return xi.job_utils.beastmaster.msgCannotCharm
end

-- Pure: RewardHealing
-- params: foodItemId, playerMND, rewardHPBonus, petMissingHP
-- returns: total, ok
xi.job_utils.beastmaster.rewardHealingFromParams = function(params)
    params = params or {}
    local foodData = xi.job_utils.beastmaster.petFoodData[params.foodItemId]
    if not foodData then
        return 0, false
    end

    local total = foodData.minHealing + foodData.mndMult * ((params.playerMND or 0) - foodData.mndThreshold)
    -- integer product (Lua floor of int arith)
    total = math.floor(total)

    if (params.rewardHPBonus or 0) > 0 then
        total = total + math.floor(total * (params.rewardHPBonus or 0) / 100)
    end

    local missing = params.petMissingHP or 0
    if missing < 0 then
        missing = 0
    end

    if total > missing then
        total = missing
    end

    return total, true
end

-- Pure: RewardRegen power
xi.job_utils.beastmaster.rewardRegenFromParams = function(foodItemId)
    local foodData = xi.job_utils.beastmaster.petFoodData[foodItemId]
    if not foodData then
        return 0, false
    end

    return foodData.regen, true
end

-- Pure: StayHealingTick
xi.job_utils.beastmaster.stayHealingTickFromParams = function(bstLevel)
    bstLevel = bstLevel or 0
    if bstLevel < 0 then
        bstLevel = 0
    end

    local tick = xi.job_utils.beastmaster.stayTickMax - math.ceil(math.max(0, bstLevel / 20))
    if tick < xi.job_utils.beastmaster.stayTickMin then
        return xi.job_utils.beastmaster.stayTickMin
    end

    if tick > xi.job_utils.beastmaster.stayTickMax then
        return xi.job_utils.beastmaster.stayTickMax
    end

    return tick
end

-- Pure: KillerInstinctDuration
xi.job_utils.beastmaster.killerInstinctDurationFromParams = function(meritValue)
    return xi.job_utils.beastmaster.killerInstinctBaseDuration
        + ((meritValue or 0) - xi.job_utils.beastmaster.killerInstinctMeritUnit)
end

-- Pure: SpurPowers — returns power, subPower
xi.job_utils.beastmaster.spurPowersFromParams = function(params)
    params = params or {}
    local power = xi.job_utils.beastmaster.spurBasePower + (params.enhancesSpur or 0)
    local subPower = (params.spurJP or 0) * xi.job_utils.beastmaster.spurJPAttackPerLevel
    return power, subPower
end

-- Pure: FeralHowlDuration (pre-resist)
xi.job_utils.beastmaster.feralHowlDurationFromParams = function(params)
    params = params or {}
    local duration = xi.job_utils.beastmaster.feralHowlBaseDuration
    if (params.feralHowlDurationMod or 0) >= 1 then
        duration = duration + math.floor((params.meritValue or 0) / xi.job_utils.beastmaster.feralHowlMeritUnit)
    end

    return duration
end

-- Pure: OneHourRecast
xi.job_utils.beastmaster.oneHourRecastFromParams = function(params)
    params = params or {}
    local recast = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.beastmaster.oneHourRecastSecondsPerMod
    if recast < 0 then
        return 0
    end

    return recast
end

-----------------------------------
-- Helper Functions
-----------------------------------

local function getCharmDuration(charmer, target)
    return xi.job_utils.beastmaster.charmDurationFromParams({
        charmerCHR   = charmer:getStat(xi.mod.CHR),
        charmerLevel = charmer:getMainLvl(),
        targetLevel  = target:getMainLvl(),
        charmTimeMod = charmer:getMod(xi.mod.CHARM_TIME),
    })
end

local getValidJugPetID = function(player)
    -- jug pet reagents are
    -- - equipped in the ammo slot
    -- - have skillid 0
    -- - the subskill maps to the jug petid
    local ammoEquip = player:getEquippedItem(xi.slot.AMMO)
    return xi.job_utils.beastmaster.validJugPetIDFromParams({
        ammoSubSkill    = player:getWeaponSubSkillType(xi.slot.AMMO),
        ammoSkill       = player:getWeaponSkillType(xi.slot.AMMO),
        ammoPresent     = ammoEquip ~= nil,
        ammoReqLevel    = ammoEquip and ammoEquip:getReqLvl() or 0,
        playerMainLevel = player:getMainLvl(),
    })
end

xi.job_utils.beastmaster.getCharmChance = function(charmer, target, includeMods)
    local eligible =
        charmer and
        target and
        charmer:isPC() and
        target:isMob() and
        target:getMobMod(xi.mobMod.CHARMABLE) ~= 0 and
        target:getMaster() == nil

    return xi.job_utils.beastmaster.charmChanceFromParams({
        eligible        = eligible,
        charmerBSTLevel = eligible and charmer:getJobLevel(xi.job.BST) or 0,
        targetLevel     = eligible and target:getMainLvl() or 0,
        charmRes        = eligible and target:getMod(xi.mod.CHARMRES) or 0,
        lightResRank    = eligible and target:getMod(xi.mod.LIGHT_RES_RANK) or 0,
        includeMods     = includeMods,
        charmChanceMod  = (eligible and includeMods) and charmer:getMod(xi.mod.CHARM_CHANCE) or 0,
        charmerCHR      = eligible and charmer:getStat(xi.mod.CHR) or 0,
        targetCHR       = eligible and target:getStat(xi.mod.CHR) or 0,
    })
end

xi.job_utils.beastmaster.attemptCharm = function(charmer, target)
    if
        not charmer or         -- Invalid charmer
        not target or          -- Invalid target
        not charmer:isPC() or  -- Charmer not a player
        not (target:isMob() or -- Target not a mob or PC
        target:isPC())
    then
        return xi.msg.basic.JA_MISS
    elseif -- Not charmable so apply bind
        target:getMobMod(xi.mobMod.CHARMABLE) == 0 or -- Target is not charmable
        target:isPC() or                              -- Target is a PC (ballista)
        target:getMaster()                            -- Target already has a master
    then
        local resist = applyResistanceAddEffect(charmer, target, xi.element.ICE, 0)
        if not target:hasStatusEffect(xi.effect.BIND) and resist >= 0.5 then
            target:addStatusEffect(xi.effect.BIND, { power = 1, duration = math.random(1, 5), origin = charmer })
            return xi.msg.basic.JA_ENFEEB_IS
        else
            return xi.msg.basic.JA_MISS
        end
    end

    -- Calculate charm chance
    local chance = xi.job_utils.beastmaster.getCharmChance(charmer, target, true)

    -- If successful then calculate duration and charm
    if chance > math.random(1, 100) then
        local duration = getCharmDuration(charmer, target)

        if duration > 0 then
            charmer:charm(target, duration)
            return xi.msg.basic.CHARM_SUCCESS
        end
    end

    return xi.msg.basic.CHARM_FAIL
end

-----------------------------------
-- Ability Check Functions
-----------------------------------

xi.job_utils.beastmaster.checkCallBeast = function(player, target, ability)
    local petId = getValidJugPetID(player)

    if player:getPet() ~= nil then
        return xi.msg.basic.ALREADY_HAS_A_PET, 0
    elseif not petId then
        return xi.msg.basic.NO_JUG_PET_ITEM, 0
    elseif not player:canUseMisc(xi.zoneMisc.PET) then
        return xi.msg.basic.CANT_BE_USED_IN_AREA, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkBestialLoyalty = function(player, target, ability)
    local petId = getValidJugPetID(player)

    if player:getPet() ~= nil then
        return xi.msg.basic.ALREADY_HAS_A_PET, 0
    elseif not petId then
        return xi.msg.basic.NO_JUG_PET_ITEM, 0
    elseif not player:canUseMisc(xi.zoneMisc.PET) then
        return xi.msg.basic.CANT_BE_USED_IN_AREA, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkFamiliar = function(player, target, ability)
    local pet = player:getPet()

    if not pet then
        return xi.msg.basic.REQUIRES_A_PET, 0
    elseif
        pet:getLocalVar('hasFamiliarBuffs') == 1 or
        (not player:hasJugPet() and not pet:isCharmed())
    then
        return xi.msg.basic.NO_EFFECT_ON_PET, 0
    end

    ability:setRecast(xi.job_utils.beastmaster.oneHourRecastFromParams({
        abilityRecast     = ability:getRecast(),
        oneHourRecastMod  = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

xi.job_utils.beastmaster.checkCharm = function(player, target, ability)
    if player:getPet() ~= nil then
        return xi.msg.basic.ALREADY_HAS_A_PET, 0
    elseif
        target:getMaster() ~= nil and
        target:getMaster():isPC()
    then
        return xi.msg.basic.THAT_SOMEONES_PET, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkGauge = function(player, target, ability)
    if player:getPet() ~= nil then
        return xi.msg.basic.ALREADY_HAS_A_PET, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkTame = function(player, target, ability)
    if player:getPet() ~= nil then
        return xi.msg.basic.ALREADY_HAS_A_PET, 0
    end

    for _, member in pairs(player:getPartyWithTrusts()) do
        if member:isTrust() then
            return xi.msg.basic.UNABLE_TO_USE_JA, 0
        end
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkReward = function(player, target, ability)
    local pet = player:getPet()

    if not pet then
        return xi.msg.basic.REQUIRES_A_PET, 0 --TODO this currently will not hit this function. Returns You cannot attack that target. Targetfind.cpp line 564
    elseif
        not player:hasJugPet() and
        pet:getObjType() ~= xi.objType.MOB
    then
        return xi.msg.basic.NO_EFFECT_ON_PET, 0
    else
        local id = player:getEquipID(xi.slot.AMMO)
        if
            id >= xi.item.PET_FOOD_ALPHA_BISCUIT and
            id <= xi.item.PET_FOOD_THETA_BISCUIT
        then
            return 0, 0
        else
            return xi.msg.basic.MUST_HAVE_FOOD, 0
        end
    end
end

xi.job_utils.beastmaster.checkUnleash = function(player, target, ability)
    ability:setRecast(xi.job_utils.beastmaster.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

-- On Ability Check For Leave, Heel and Stay.
xi.job_utils.beastmaster.checkPetCommand = function(player, target, ability)
    local pet = player:getPet()

    if
        player:hasJugPet() or
        pet:getObjType() == xi.objType.MOB
    then
        if player:getPet() == nil then
            return xi.msg.basic.REQUIRES_A_PET, 0
        end
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkFight = function(player, target, ability)
    if player:getPet() == nil then
        return xi.msg.basic.REQUIRES_A_PET, 0
    elseif
        target:getID() == player:getPet():getID() or
        (target:getMaster() ~= nil and target:getMaster():isPC())
    then
        return xi.msg.basic.CANNOT_ATTACK_TARGET, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkKillerInstinct = function(player, target, ability)
    local pet = player:getPet()

    if
        pet == nil or                                                   -- No pet currently spawned
        (not player:hasJugPet() and pet:getObjType() ~= xi.objType.MOB) -- The pet spawned is not a jug pet or charmed mob
    then
        return xi.msg.basic.REQUIRES_A_PET, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkSnarl = function(player, target, ability)
    if player:getPet() == nil then
        return xi.msg.basic.REQUIRES_A_PET, 0
    else
        if
            player:getPet():getTarget() ~= nil and
            player:hasJugPet()
        then
            return 0, 0
        else
            return xi.msg.basic.PET_CANNOT_DO_ACTION, 0
        end
    end
end

xi.job_utils.beastmaster.checkSic = function(player, target, ability)
    local pet = player:getPet()

    if pet == nil then
        return xi.msg.basic.REQUIRES_A_PET, 0
    elseif
        pet:getHP() == 0 or
        not pet:hasTPMoves()
    then
        return xi.msg.basic.UNABLE_TO_USE_JA, 0
    elseif pet:getTarget() == nil then
        return xi.msg.basic.PET_CANNOT_DO_ACTION, 0
    end

    return 0, 0
end

xi.job_utils.beastmaster.checkFeralHowl = function(player, target, ability)
    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------

xi.job_utils.beastmaster.useCallBeast = function(player, target, ability)
    local petId = getValidJugPetID(player)
    if not petId then
        return
    end

    xi.pet.spawnPet(player, petId)
    player:removeAmmo(1)

    -- Briefly put the recastId for READY/SIC (102) into a recast state to
    -- toggle charges accumulating. 102 is the shared recast id for all jug
    -- pet abilities and for SIC when using a charmed mob.
    -- see sql/abilities_charges and sql_abilities
    player:addRecast(xi.recast.ABILITY, 102, 1)
end

xi.job_utils.beastmaster.useBestialLoyalty = function(player, target, ability)
    local petId = getValidJugPetID(player)
    if not petId then
        return
    end

    xi.pet.spawnPet(player, petId)

    player:addRecast(xi.recast.ABILITY, 102, 1)
end

xi.job_utils.beastmaster.useFamiliar = function(player, target, ability, action)
    local pet = player:getPet()

    xi.pet.applyFamiliarBuffs(player, pet)

    -- Redirect animation from player to pet
    action:ID(player:getID(), pet:getID())

    ability:setMsg(xi.msg.basic.FAMILIAR_PC)

    return 0
end

xi.job_utils.beastmaster.useCharm = function(player, target, ability)
    local isTamed = false

    if player:getLocalVar('Tamed_Mob') == target:getID() then
        player:addMod(xi.mod.CHARM_CHANCE, 10)
        isTamed = true
    end

    -- attempt the charm and get the return message
    local msg = xi.job_utils.beastmaster.attemptCharm(player, target)
    ability:setMsg(msg)

    if isTamed then
        player:delMod(xi.mod.CHARM_CHANCE, 10)
        player:setLocalVar('Tamed_Mob', 0)
    end

    -- if charm bound mob then need to return bind to generate correct message
    if msg == xi.msg.basic.JA_ENFEEB_IS then
        return xi.effect.BIND
    end
end

xi.job_utils.beastmaster.useGauge = function(player, target, ability)
    local charmChance = xi.job_utils.beastmaster.getCharmChance(player, target, false)
    ability:setMsg(xi.job_utils.beastmaster.gaugeMessageFromParams(charmChance))
end

-- **NOTE** Use of Battlemod may remove message
xi.job_utils.beastmaster.useTame = function(player, target, ability)
    if player:getPet() ~= nil then
        ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
        target:addEnmity(player, 1, 0)

        return 0
    end

    if target:getMobMod(xi.mobMod.CHARMABLE) == 0 then
        ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
        target:addEnmity(player, 1, 0)

        return 0
    end

    local resist = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, 0, xi.element.NONE, xi.mod.INT, 0, 0)

    if resist <= 0.25 then
        ability:setMsg(xi.msg.basic.JA_MISS_2)
        target:addEnmity(player, 1, 0)

        return 0
    else
        if target:isEngaged() then
            local enmitylist = target:getEnmityList()

            for _, enmity in ipairs(enmitylist) do
                if enmity.active and enmity.entity:getID() ~= player:getID() then
                    ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
                    target:addEnmity(player, 1, 0)

                    return 0
                elseif enmity.entity:getID() == player:getID() then
                    if not enmity.tameable then
                        ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
                        target:addEnmity(player, 1, 0)

                        return 0
                    end
                end
            end

            ability:setMsg(138) -- The x seems friendlier
            target:disengage()
        else
            player:setLocalVar('Tamed_Mob', target:getID())
            ability:setMsg(138) -- The x seems friendlier
        end
    end
end

xi.job_utils.beastmaster.useReward = function(player, target, ability)
    -- 1st need to get the pet food is equipped in the range slot.
    local rangeObj         = player:getEquipID(xi.slot.AMMO)
    local rewardHealingMod = player:getMod(xi.mod.REWARD_HP_BONUS)
    local pet              = player:getPet()
    local petCurrentHP     = pet:getHP()
    local petMaxHP         = pet:getMaxHP()

    -- Please note that I used this as base for the calculations:
    -- http://wiki.ffxiclopedia.org/wiki/Reward

    local totalHealing, healOk = xi.job_utils.beastmaster.rewardHealingFromParams({
        foodItemId    = rangeObj,
        playerMND     = player:getStat(xi.mod.MND),
        rewardHPBonus = rewardHealingMod or 0,
        petMissingHP  = petMaxHP - petCurrentHP,
    })
    if not healOk then
        totalHealing = 0
    end

    local regenAmount, regenOk = xi.job_utils.beastmaster.rewardRegenFromParams(rangeObj)
    if not regenOk then
        regenAmount = 1 -- 1 is the minimum host default when food missing
    end

    local regenTime = xi.job_utils.beastmaster.rewardRegenDuration

    -- Now calculating the bonus based on gear.
    switch(player:getEquipID(xi.slot.BODY)):caseof
    {
        [xi.item.BEAST_JACKCOAT] = function() -- beast jackcoat
            -- This will remove Paralyze, Poison and Blind from the pet.
            pet:delStatusEffect(xi.effect.PARALYSIS)
            pet:delStatusEffect(xi.effect.POISON)
            pet:delStatusEffect(xi.effect.BLINDNESS)
        end,

        [xi.item.BEAST_JACKCOAT_P1] = function() -- beast jackcoat +1
            -- This will remove Paralyze, Poison, Blind, Weight, Slow and Silence from the pet.
            pet:delStatusEffect(xi.effect.PARALYSIS)
            pet:delStatusEffect(xi.effect.POISON)
            pet:delStatusEffect(xi.effect.BLINDNESS)
            pet:delStatusEffect(xi.effect.WEIGHT)
            pet:delStatusEffect(xi.effect.SLOW)
            pet:delStatusEffect(xi.effect.SILENCE)
        end,

        [xi.item.MONSTER_JACKCOAT] = function() -- monster jackcoat
            -- This will remove Weight, Slow and Silence from the pet.
            pet:delStatusEffect(xi.effect.WEIGHT)
            pet:delStatusEffect(xi.effect.SLOW)
            pet:delStatusEffect(xi.effect.SILENCE)
        end,

        [xi.item.MONSTER_JACKCOAT_P1] = function() -- monster jackcoat +1
            -- This will remove Paralyze, Poison, Blind, Weight, Slow and Silence from the pet.
            pet:delStatusEffect(xi.effect.PARALYSIS)
            pet:delStatusEffect(xi.effect.POISON)
            pet:delStatusEffect(xi.effect.BLINDNESS)
            pet:delStatusEffect(xi.effect.WEIGHT)
            pet:delStatusEffect(xi.effect.SLOW)
            pet:delStatusEffect(xi.effect.SILENCE)
        end,
    }

    pet:addHP(totalHealing)
    pet:wakeUp()

    -- Apply regen xi.effect.

    pet:delStatusEffect(xi.effect.REGEN)
    pet:addStatusEffect(xi.effect.REGEN, {
        power    = regenAmount,
        duration = regenTime,
        origin   = player,
        tick     = xi.job_utils.beastmaster.rewardRegenTick,
    })
    player:removeAmmo(1)

    pet:updateEnmityFromCure(pet, totalHealing)

    return totalHealing
end

xi.job_utils.beastmaster.useUnleash = function(player, target, ability)
    player:addStatusEffect(xi.effect.UNLEASH, {
        power    = xi.job_utils.beastmaster.unleashPower,
        duration = xi.job_utils.beastmaster.unleashDuration,
        origin   = player,
    })

    return xi.effect.UNLEASH
end

xi.job_utils.beastmaster.useLeave = function(player, target, ability)
    local pet = target:getPet()

    if
        pet and
        pet:hasStatusEffect(xi.effect.HEALING)
    then
        pet:delStatusEffect(xi.effect.HEALING)
    end

    target:despawnPet()
end

xi.job_utils.beastmaster.useSnarl = function(player, target, ability)
    player:transferEnmity(player:getPet(), 99, 11.5)
end

xi.job_utils.beastmaster.useSic = function(player, target, ability)
    local function doSic(mob)
        if mob:getTP() >= 1000 then
            mob:useMobAbility()
        elseif mob:hasSpellList() then
            mob:castSpell()
        else
            mob:queue(0, doSic)
        end
    end

    player:getPet():queue(0, doSic)
end

xi.job_utils.beastmaster.useHeel = function(player, target, ability)
    local pet = player:getPet()

    if pet:hasStatusEffect(xi.effect.HEALING) then
        pet:delStatusEffect(xi.effect.HEALING)
    end

    player:petRetreat()
end

xi.job_utils.beastmaster.useStay = function(player, target, ability)
    local pet = player:getPet()

    if not pet:hasPreventActionEffect() then
        -- reduce tick speed based on level. but never less than 5 and never
        -- more than 10.  This seems to mimic retail.  There is no formula
        -- that I can find, but this seems close.
        local level = 0
        if player:getMainJob() == xi.job.BST then
            level = player:getMainLvl()
        elseif player:getSubJob() == xi.job.BST then
            level = player:getSubLvl()
        end

        local tick = xi.job_utils.beastmaster.stayHealingTickFromParams(level)

        pet:addStatusEffect(xi.effect.HEALING, { origin = player, tick = tick, icon = 0 })
        pet:setAnimation(0)
    end
end

xi.job_utils.beastmaster.useFight = function(player, target, ability)
    local pet = player:getPet()

    if player:checkDistance(pet) <= 25 then
        if pet:hasStatusEffect(xi.effect.HEALING) then
            pet:delStatusEffect(xi.effect.HEALING)
        end

        player:petAttack(target)
    end
end

xi.job_utils.beastmaster.useKillerInstinct = function(player, target, ability, action)
    -- Notes: Pet ecosystem is assigned to the subPower, then mapped to the correct killer mod in the effect script.
    local pet          = player:getPet()
    local petEcosystem = pet:getEcosystem()
    local power        = xi.job_utils.beastmaster.killerInstinctPower
    local duration     = xi.job_utils.beastmaster.killerInstinctDurationFromParams(
        player:getMerit(xi.merit.KILLER_INSTINCT)
    )

    target:addStatusEffect(xi.effect.KILLER_INSTINCT, {
        power    = power,
        duration = duration,
        origin   = player,
        subPower = petEcosystem,
    })

    return xi.effect.KILLER_INSTINCT
end

xi.job_utils.beastmaster.useSpur = function(player)
    local power, subpower = xi.job_utils.beastmaster.spurPowersFromParams({
        enhancesSpur = player:getMod(xi.mod.ENHANCES_SPUR),
        spurJP       = player:getJobPointLevel(xi.jp.SPUR_EFFECT),
    })
    local pet = player:getPet()
    if pet then
        pet:addStatusEffect(xi.effect.SPUR, {
            power    = power,
            duration = xi.job_utils.beastmaster.spurDuration,
            origin   = player,
            subPower = subpower,
        })
    end
end

xi.job_utils.beastmaster.useRunWild = function(player, target, ability, action)
    -- all but regen are a 25% bonus
    local power = 25
    local pet = player:getPet()
    if pet then
        -- mods aren't tied to an effect, just applied to the pet. They leave when the pet dies or despawns
        pet:addMod(xi.mod.ATTP, power)
        pet:addMod(xi.mod.ACC, pet:getACC() * power / 100)
        -- Yep, it's an MAB % addition
        -- "If you have no sources of Magic Attack Bonus while using the slug pet, then Run Wild actually makes his innate MAB penalty even more negative, thus reducing damage."
        pet:addMod(xi.mod.MATT, pet:getMod(xi.mod.MATT) * power / 100)
        pet:addMod(xi.mod.EVA, pet:getEVA() * power / 100)
        pet:addMod(xi.mod.DEFP, power)
        -- TODO find out this potency, but appears to be consistently 1% per tick with hare familiar at lvl 99
        pet:addMod(xi.mod.REGEN, 0.01 * pet:getMaxHP())

        -- After 5 minutes, the pet just despawns
        pet:setJugRemainingTime(300)
    end

    -- seems to display nothing in console, but this it the msg id from capture
    ability:setMsg(154)

    return ability:getID()
end

xi.job_utils.beastmaster.useFeralHowl = function(player, target, ability, action)
    local modAcc       = player:getMerit(xi.merit.FERAL_HOWL)
    local feralHowlMod = player:getMod(xi.mod.FERAL_HOWL_DURATION)
    local duration     = xi.job_utils.beastmaster.feralHowlDurationFromParams({
        meritValue           = modAcc,
        feralHowlDurationMod = feralHowlMod,
    })

    if
        xi.data.statusEffect.isTargetImmune(target, xi.effect.TERROR, xi.element.DARK) or
        xi.data.statusEffect.isTargetResistant(player, target, xi.effect.TERROR) or
        xi.data.statusEffect.isEffectNullified(target, xi.effect.TERROR, 0)
    then
        ability:setMsg(xi.msg.basic.JA_MISS_2)
    else
        -- modAcc returns 5 per merit level (5, 10, 15, 20, 25), providing 5% accuracy bonus per merit
        local resistanceRate = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, xi.skillRank.B_MINUS, xi.element.DARK, xi.mod.CHR, xi.effect.TERROR, modAcc)

        if xi.data.statusEffect.isResistRateSuccessfull(xi.effect.TERROR, resistanceRate, 0) then
            target:addStatusEffect(xi.effect.TERROR, { power = 1, duration = duration * resistanceRate, origin = player })
        end
    end

    return xi.effect.TERROR
end
