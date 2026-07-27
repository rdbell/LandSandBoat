-----------------------------------
-- Global version of onMobDeath
-----------------------------------
require('scripts/globals/magic')
require('scripts/globals/missions')
require('scripts/globals/quests')
-----------------------------------
xi = xi or {}
xi.mob = xi.mob or {}

-- onMobDeathEx is called from the core
xi.mob.onMobDeathEx = function(mob, player, isKiller, isWeaponSkillKill)
end

-----------------------------------
-- placeholder / lottery NMs
-----------------------------------

-- Pure halves of xi.mob.phOnDespawn, extracted so the lottery decision is
-- testable without entity, settings, or RNG access.

-- Every NM id reachable from a placeholder list, flattening the nested
-- { id, id2 } entries lotteryPrimed walks. Order follows the outer list.
xi.mob.phListNmIds = function(phList)
    local ids = {}

    if not phList then
        return ids
    end

    for _, entry in pairs(phList) do
        if type(entry) == 'table' then
            for _, innerId in pairs(entry) do
                table.insert(ids, innerId)
            end
        else
            table.insert(ids, entry)
        end
    end

    return ids
end

-- An NM is primed when it is already spawned or has a pending respawn.
xi.mob.nmPrimed = function(isSpawned, respawnTime)
    return isSpawned or respawnTime ~= 0
end

-- The NM ids a placeholder can pop. A plain number yields one candidate; a
-- table yields all of them for the caller to choose between.
xi.mob.nmCandidatesForPh = function(phList, phId)
    local entry = phList and phList[phId]

    if type(entry) == 'number' then
        return { entry }
    elseif type(entry) == 'table' then
        local ids = {}
        for _, id in pairs(entry) do
            table.insert(ids, id)
        end

        return ids
    end

    return {}
end

-- NM_LOTTERY_CHANCE scaling, then the x10 conversion to a 1..1000 roll domain.
-- A negative setting forces 100 (a guaranteed pop); an absent setting leaves the
-- script's own chance untouched.
xi.mob.lotteryScaledChance = function(chance, setting)
    if setting then
        chance = setting >= 0 and (chance * setting) or 100
    end

    return math.ceil(chance * 10)
end

-- NM_LOTTERY_COOLDOWN scaling. A negative setting leaves the cooldown alone.
xi.mob.lotteryScaledCooldown = function(cooldown, setting)
    if setting then
        cooldown = setting >= 0 and (cooldown * setting) or cooldown
    end

    return cooldown
end

-- The Vana'diel hour a placeholder's next repop lands on.
xi.mob.lotteryRepopHour = function(vanadielTime, phRespawnTime)
    return math.floor(((vanadielTime + phRespawnTime) % xi.vanaTime.DAY) / xi.vanaTime.HOUR)
end

-- Day/night restriction on the repop hour.
--
-- NOTE: the dayOnly arm requires hour < 4 AND hour >= 20, which no hour
-- satisfies, so dayOnly never blocks a pop upstream. Preserved deliberately for
-- parity; the nightOnly arm does work.
xi.mob.lotteryRepopBlocked = function(dayOnly, nightOnly, nextRepopHour)
    if
        dayOnly and
        nextRepopHour < 4 and
        nextRepopHour >= 20
    then
        return true
    elseif
        nightOnly and
        nextRepopHour >= 4 and
        nextRepopHour < 20
    then
        return true
    end

    return false
end

-- The NM's post-kill cooldown is still running.
xi.mob.lotteryCooldownActive = function(now, popUntil)
    return now <= popUntil
end

-- A 1..1000 roll hits when it lands at or under the scaled chance.
xi.mob.lotteryRollPassed = function(roll, scaledChance)
    return roll <= scaledChance
end

-- The three-way pop gate: cooldown still running, another NM already primed, or
-- the roll missed. Callers that must not draw from the shared RNG unless the
-- earlier gates pass should short-circuit over the two primitives instead.
xi.mob.lotteryPopAdmitted = function(now, popUntil, primed, roll, scaledChance)
    return not xi.mob.lotteryCooldownActive(now, popUntil) and
        not primed and
        xi.mob.lotteryRollPassed(roll, scaledChance)
end

-- Respawn delay applied to the NM once the pop is admitted.
xi.mob.lotteryRespawnTime = function(immediate, phRespawnTime)
    return immediate and 1 or phRespawnTime
end

-- is a lottery NM in the table already spawned or primed to pop?
local function lotteryPrimed(phList)
    for _, nmId in ipairs(xi.mob.phListNmIds(phList)) do
        local nm = GetMobByID(nmId)

        if nm ~= nil and xi.mob.nmPrimed(nm:isSpawned(), nm:getRespawnTime()) then
            return true
        end
    end

    return false
end

local function getMobLuaPathObject(mob)
    if not mob then
        return nil
    end

    return xi.zones[mob:getZoneName()].mobs[mob:getName()]
end

-- - mobParam can either be a mobid or a mob entity object
-- it either accepts a table of spawn points to randomize, or looks to the mob's cached lua object for a spawnPoints entry
---@param mobParam number|CBaseEntity?
---@param spawnPointsOverride table?
xi.mob.updateNMSpawnPoint = function(mobParam, spawnPointsOverride)
    local origMobParam = mobParam
    -- sometimes we call from Zone.lua files and only have the mob id
    if type(mobParam) == 'number' then
        mobParam = GetMobByID(mobParam)
    end

    if mobParam == nil then
        print('[updateNMSpawnPoint] Invalid mob parameter:')
        print(origMobParam)

        return
    end

    -- if no spawnPoints table was sent, extract from mob lua object
    local spawnPoints = spawnPointsOverride
    if spawnPoints == nil then
        local mobObject = getMobLuaPathObject(mobParam)
        if mobObject and mobObject.spawnPoints then
            spawnPoints = mobObject.spawnPoints
        end
    end

    -- Special check for NMs with the same name but multiple IDs
    if spawnPoints and spawnPoints[mobParam:getID()] then
        spawnPoints = spawnPoints[mobParam:getID()]
    end

    if
        spawnPoints ~= nil and
        type(spawnPoints) == 'table' and
        #spawnPoints > 0
    then
        local chosenSpawn    = utils.randomEntry(spawnPoints)
        local randomRotation = math.random(0, 255) -- rotation does not matter

        -- Updates the mob's spawn point
        mobParam:setSpawn(chosenSpawn.x, chosenSpawn.y, chosenSpawn.z, randomRotation)
    end
end

local function getMobEntityObj(phNmId)
    local mobEntityObj = nil

    if type(phNmId) == 'number' then
        mobEntityObj = getMobLuaPathObject(GetMobByID(phNmId))
    elseif type(phNmId) == 'table' then
        mobEntityObj = getMobLuaPathObject(GetMobByID(utils.randomEntry(phNmId)))
    end

    return mobEntityObj
end

local function getNmId(phList, phId)
    local candidates = xi.mob.nmCandidatesForPh(phList, phId)

    if #candidates == 0 then
        return nil
    end

    return utils.randomEntry(candidates)
end

-- potential lottery placeholder was killed
---@param ph CBaseEntity
---@param phNmId integer|table
---@param chance integer
---@param cooldown integer
---@param params table?
xi.mob.phOnDespawn = function(ph, phNmId, chance, cooldown, params)
    params = params or {}
    --[[
        params.immediate          = true    pop NM without waiting for next PH pop time
        params.dayOnly            = true    spawn NM only at day time
        params.nightOnly          = true    spawn NM only at night time
        params.noPosUpdate        = true    do not run xi.mob.updateNMSpawnPoint()
        params.spawnPoints        = { {x = , y = , z = } } table of spawn points to choose from, overrides NM's lua-defined table
        params.doNotEnablePhSpawn = true    Don't enable ph respawns after NM is killed (for chained ph systems like steelfleece)
    ]]

    local phId         = ph:getID()
    local nmId         = nil
    local nm           = nil
    local phList       = nil
    local mobEntityObj = getMobEntityObj(phNmId)

    if mobEntityObj then
        phList = mobEntityObj.phList
        nmId   = getNmId(phList, phId)
        nm     = nmId and GetMobByID(nmId)
    end

    -- This was not a PH for the NM
    if
        type(nmId) ~= 'number' or
        nm == nil or
        phList == nil
    then
        return false
    end

    -- ensure certain boolean params exist
    local paramKeys =
    {
        'immediate',
        'dayOnly',
        'nightOnly',
        'noPosUpdate',
        'doNotEnablePhSpawn',
    }

    for _, pKey in ipairs(paramKeys) do
        if type(params[pKey]) ~= 'boolean' then
            params[pKey] = false
        end
    end

    chance   = xi.mob.lotteryScaledChance(chance, xi.settings.main.NM_LOTTERY_CHANCE)
    cooldown = xi.mob.lotteryScaledCooldown(cooldown, xi.settings.main.NM_LOTTERY_COOLDOWN)

    -- Short-circuit over the primitives rather than calling lotteryPopAdmitted:
    -- upstream only draws from the shared RNG once the cooldown and primed
    -- gates have both passed.
    if
        xi.mob.lotteryCooldownActive(GetSystemTime(), nm:getLocalVar('pop')) or
        lotteryPrimed(phList) or
        not xi.mob.lotteryRollPassed(math.random(1, 1000), chance)
    then
        return false
    end

    local nextRepopHour = xi.mob.lotteryRepopHour(VanadielTime(), GetMobRespawnTime(phId))

    if xi.mob.lotteryRepopBlocked(params.dayOnly, params.nightOnly, nextRepopHour) then
        return false
    end

    -- on PH death, replace PH repop with NM repop
    -- TODO, fetch phId's spawn slot and disable respawn for all mobs in that spawn slot
    DisallowRespawn(phId, true)
    DisallowRespawn(nmId, false)

    -- Update mob's spawn position, if available
    if not params.noPosUpdate then
        xi.mob.updateNMSpawnPoint(nm, params.spawnPoints or nil)
    end

    -- if params.immediate is true, spawn the nm params.immediately (1ms) else use placeholder's timer
    nm:setRespawnTime(xi.mob.lotteryRespawnTime(params.immediate, GetMobRespawnTime(phId)))

    nm:addListener('DESPAWN', 'DESPAWN_' .. nmId, function(m)
        -- on NM death, replace NM repop with PH repop
        DisallowRespawn(nmId, true)
        if not params.doNotEnablePhSpawn then
            DisallowRespawn(phId, false)
            local phMob = GetMobByID(phId)
            if phMob then
                phMob:setRespawnTime(GetMobRespawnTime(phId))
            end
        end

        if m:getLocalVar('doNotInvokeCooldown') == 0 then
            m:setLocalVar('pop', GetSystemTime() + cooldown)
        end

        m:removeListener('DESPAWN_' .. nmId)
    end)

    return true
end

-----------------------------------
-- mob additional melee effects
-----------------------------------

xi.mob.ae =
{
    HP_DRAIN     = 11,
    MP_DRAIN     = 12,
    PETRIFY      = 14,
    PLAGUE       = 15,
    POISON       = 16,
    SILENCE      = 17,
    SLOW         = 18,
    STUN         = 19,
    TERROR       = 20,
    TP_DRAIN     = 21,
    WEIGHT       = 22,
    ENAMNESIA    = 23,
    BIND         = 25,
    SLEEP        = 26,
    DEFENSE_DOWN = 27,
    ATTACK_DOWN  = 28,
}

local additionalEffects =
{
    [xi.mob.ae.HP_DRAIN] =
    {
        chance             = 10,
        ele                = xi.element.DARK,
        sub                = xi.subEffect.HP_DRAIN,
        msg                = xi.msg.basic.ADD_EFFECT_HP_DRAIN,
        mod                = xi.mod.INT,
        bonusAbilityParams = { bonusmab = 0, includemab = false },
        code               = function(mob, target, power)
            mob:addHP(power)
        end,
    },

    [xi.mob.ae.MP_DRAIN] =
    {
        chance             = 10,
        ele                = xi.element.DARK,
        sub                = xi.subEffect.MP_DRAIN,
        msg                = xi.msg.basic.ADD_EFFECT_MP_DRAIN,
        mod                = xi.mod.INT,
        bonusAbilityParams = { bonusmab = 0, includemab = false },
        code               = function(mob, target, power)
            local mp = math.min(power, target:getMP())
            target:delMP(mp)
            mob:addMP(mp)
        end,
    },

    [xi.mob.ae.PETRIFY] =
    {
        chance      = 20,
        ele         = xi.element.EARTH,
        sub         = xi.subEffect.PETRIFY,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.PETRIFICATION,
        power       = 1,
        duration    = 30,
        minDuration = 1,
        maxDuration = 45,
    },

    [xi.mob.ae.PLAGUE] =
    {
        chance      = 25,
        ele         = xi.element.WATER,
        sub         = xi.subEffect.PLAGUE,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.PLAGUE,
        power       = 1,
        duration    = 60,
        minDuration = 1,
        maxDuration = 60,
    },

    [xi.mob.ae.POISON] =
    {
        chance      = 25,
        ele         = xi.element.WATER,
        sub         = xi.subEffect.POISON,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.POISON,
        power       = 1,
        duration    = 30,
        minDuration = 1,
        maxDuration = 30,
        tick        = 3,
    },

    [xi.mob.ae.SILENCE] =
    {
        chance      = 25,
        ele         = xi.element.WIND,
        sub         = xi.subEffect.SILENCE,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.SILENCE,
        power       = 1,
        duration    = 30,
        minDuration = 1,
        maxDuration = 30,
    },

    [xi.mob.ae.ENAMNESIA] =
    {
        chance      = 25,
        ele         = xi.element.FIRE,
        sub         = xi.subEffect.AMNESIA,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.AMNESIA,
        power       = 1,
        duration    = 30,
        minDuration = 1,
        maxDuration = 30,
    },

    [xi.mob.ae.SLEEP] =
    {
        chance      = 25,
        ele         = xi.element.DARK,
        sub         = xi.subEffect.SLEEP,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.SLEEP_I,
        power       = 20,
        duration    = 30,
        minDuration = 1,
        maxDuration = 45,
    },

    [xi.mob.ae.SLOW] =
    {
        chance      = 25,
        ele         = xi.element.EARTH,
        sub         = xi.subEffect.DEFENSE_DOWN,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.SLOW,
        power       = 1000,
        duration    = 30,
        minDuration = 1,
        maxDuration = 45,
    },

    [xi.mob.ae.STUN] =
    {
        chance      = 20,
        ele         = xi.element.THUNDER,
        sub         = xi.subEffect.STUN,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.STUN,
        duration    = 5,
    },

    [xi.mob.ae.TERROR] =
    {
        chance = 20,
        sub         = xi.subEffect.PARALYSIS,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.TERROR,
        duration    = 5,
        code        = function(mob, target, power)
            mob:resetEnmity(target)
        end,
    },

    [xi.mob.ae.TP_DRAIN] =
    {
        chance             = 25,
        ele                = xi.element.DARK,
        sub                = xi.subEffect.TP_DRAIN,
        msg                = xi.msg.basic.ADD_EFFECT_TP_DRAIN,
        mod                = xi.mod.INT,
        bonusAbilityParams = { bonusmab = 0, includemab = false },
        code               = function(mob, target, power)
            local tp = math.min(power, target:getTP())
            target:delTP(tp)
            mob:addTP(tp)
        end,
    },

    [xi.mob.ae.WEIGHT] =
    {
        chance      = 25,
        ele         = xi.element.WIND,
        sub         = xi.subEffect.ATTACK_DOWN,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.WEIGHT,
        power       = 1,
        duration    = 30,
        minDuration = 1,
        maxDuration = 45,
    },

    [xi.mob.ae.BIND] =
    {
        chance      = 10,
        ele         = xi.element.ICE,
        sub         = xi.subEffect.DARKNESS_DAMAGE,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.BIND,
        power       = 1,
        duration    = 30,
        minDuration = 1,
        maxDuration = 90,
    },

    [xi.mob.ae.DEFENSE_DOWN] =
    {
        chance      = 20,
        ele         = xi.element.WIND,
        sub         = xi.subEffect.DEFENSE_DOWN,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.DEFENSE_DOWN,
        power       = 25,
        duration    = 30,
        minDuration = 1,
        maxDuration = 60,
    },

    [xi.mob.ae.ATTACK_DOWN] =
    {
        chance      = 20,
        ele         = xi.element.WATER,
        sub         = xi.subEffect.ATTACK_DOWN,
        msg         = xi.msg.basic.ADD_EFFECT_STATUS,
        applyEffect = true,
        eff         = xi.effect.ATTACK_DOWN,
        power       = 25,
        duration    = 30,
        minDuration = 1,
        maxDuration = 60,
    },
}

-- Pure halves of xi.mob.onAddEffect, extracted so the proc and power maths are
-- testable without entity or RNG access.

-- The additional-effect definition for an xi.mob.ae value, or nil when unknown.
xi.mob.additionalEffectData = function(effect)
    return additionalEffects[effect]
end

-- Proc chance after the level-difference penalty. A target above the mob loses
-- 5 points per level and the result is clamped to 5..95; a target at or below
-- the mob's level keeps the unclamped base chance.
xi.mob.addEffectProcChance = function(paramsChance, aeChance, dLevel)
    local chance = paramsChance or aeChance or 100

    if dLevel > 0 then
        chance = chance - 5 * dLevel
        chance = utils.clamp(chance, 5, 95)
    end

    return chance
end

-- A status add-effect lands only on a weak resist and an unafflicted target.
xi.mob.addEffectStatusApplies = function(resist, targetHasEffect)
    return resist > 0.5 and not targetHasEffect
end

-- Status duration after the min/max clamp and the resist scale.
--
-- NOTE: entries without minDuration/maxDuration (STUN, TERROR) fall through
-- utils.clamp unchanged, so their duration is only scaled by resist.
xi.mob.addEffectStatusDuration = function(paramsDuration, aeDuration, minDuration, maxDuration, resist)
    local duration = paramsDuration or aeDuration

    return utils.clamp(duration, minDuration, maxDuration) * resist
end

-- Attacker-minus-target stat delta feeding an immediate add-effect's power.
-- Gains past 20 are halved and negative deltas floor at 0.
xi.mob.addEffectDMod = function(mobStat, targetStat)
    local dMod = mobStat - targetStat

    if dMod > 20 then
        dMod = 20 + (dMod - 20) / 2
    end

    -- This is a bad assumption, but it prevents some negative damage (healing)
    -- when there otherwise shouldn't be
    if dMod < 0 then
        dMod = 0
    end

    return dMod
end

-- Base power of an immediate add-effect before elemental adjustment.
xi.mob.addEffectBasePower = function(dMod, targetLvl, mobLvl, damage)
    return dMod + targetLvl - mobLvl + damage / 2
end

-- Negative power becomes a healing message where one exists, and is otherwise
-- dropped to zero. Returns the adjusted power and the message to report.
xi.mob.addEffectNegativeAdjust = function(power, msg, negMsg)
    if power >= 0 then
        return power, msg
    end

    if negMsg then
        -- outgoing action packets only support unsigned integers
        return power * -1, negMsg
    end

    return 0, msg
end

--[[
    Helper function for xi.mob.onAddEffect that applies a status effect.
--]]
local addEffectStatus = function(mob, target, ae, params)
    local resist = 1

    if ae.ele then
        resist = applyResistanceAddEffect(mob, target, ae.ele, ae.eff)
    end

    if xi.mob.addEffectStatusApplies(resist, target:hasStatusEffect(ae.eff)) then
        local power    = params.power or ae.power or 0
        local tick     = ae.tick or 0
        local duration = xi.mob.addEffectStatusDuration(params.duration, ae.duration, ae.minDuration, ae.maxDuration, resist)

        target:addStatusEffect(ae.eff, { power = power, duration = duration, origin = mob, tick = tick })

        if params.code then
            params.code(mob, target, power)
        elseif ae.code then
            ae.code(mob, target, power)
        end

        return ae.sub, ae.msg, ae.eff
    end

    return 0, 0, 0
end

--[[
    Helper function for xi.mob.onAddEffect that applies damage.
--]]
local addEffectImmediate = function(mob, target, damage, ae, params)
    local power = 0

    if params.power then
        power = params.power
    elseif ae.mod then
        -- TODO: better understand damage add effects from mobs
        power = xi.mob.addEffectBasePower(
            xi.mob.addEffectDMod(mob:getStat(ae.mod), target:getStat(ae.mod)),
            target:getMainLvl(),
            mob:getMainLvl(),
            damage
        )
    end

    -- target:printToPlayer(string.format('Initial Power: %f', power)) -- DEBUG

    power = addBonusesAbility(mob, ae.ele, target, power, ae.bonusAbilityParams)
    power = power * applyResistanceAddEffect(mob, target, ae.ele, 0)
    power = power * xi.spells.damage.calculateAbsorption(target, ae.ele, true)
    power = power * xi.spells.damage.calculateNullification(target, ae.ele, true, false)

    if ae.sub ~= xi.subEffect.TP_DRAIN and ae.sub ~= xi.subEffect.MP_DRAIN then
        power = finalMagicNonSpellAdjustments(mob, target, ae.ele, power)
    end

    -- target:printToPlayer(string.format('Adjusted Power: %f', power)) -- DEBUG

    -- The "negative message" also handles healing automagically deep inside core somewhere.
    local message
    power, message = xi.mob.addEffectNegativeAdjust(power, ae.msg, ae.negMsg)

    if power ~= 0 then
        if params.code then
            params.code(mob, target, power)
        elseif ae.code then
            ae.code(mob, target, power)
        end

        return ae.sub, message, power
    end

    return 0, 0, 0
end

--[[
    mob, target, and damage are passed from core into mob script's onAdditionalEffect
    effect should be of type xi.mob.additionalEffect (see above)
    params is a table that can contain any of:
        chance: percent chance that effect procs on hit (default 20)
        power: power of effect
        duration: duration of effect, in seconds
        code: additional code that will run when effect procs, of form function(mob, target, power)
    params will override effect's default settings
--]]
xi.mob.onAddEffect = function(mob, target, damage, effect, params)
    if type(params) ~= 'table' then
        params = {}
    end

    local ae = additionalEffects[effect]

    if ae then
        local chance = xi.mob.addEffectProcChance(
            params.chance,
            ae.chance,
            target:getMainLvl() - mob:getMainLvl()
        )

        -- target:printToPlayer(string.format('Chance: %i', chance)) -- DEBUG

        if math.random(1, 100) <= chance then

            -- STATUS EFFECT
            if ae.applyEffect then
                return addEffectStatus(mob, target, ae, params)

            -- IMMEDIATE EFFECT
            else
                return addEffectImmediate(mob, target, damage, ae, params)
            end
        end
    else
        printf('invalid additional effect for mobId %i', mob:getID())
    end

    return 0, 0, 0
end

-----------------------------------
-- Centralized function for calling one or more mob "pets"
-- It may be helpful to think of mobs with multiple as having "helpers" rather than explicitly pets
-- Since this is a looser definition than an explicit `->PMaster` and `->PPet` relationship that exists:
-- - these "pets" can have real pets of their own
-- - mobs can have multiple "pets"
-- - if the petId maps to the mob's actual pet (or petIds is nil and the mob has a pet mapped),
--      then no ROAM listener is installed on the pet, but the animations can still be consistently managed in one place
-----------------------------------

-- Pure halves of xi.mob.callPets.

-- Per-job action packet used to announce a pet call, or nil for jobs without
-- one (which fall back to the generic two-hour packet).
local callPetJobActions =
{
    [xi.job.BST] =
    {
        finishCategory = xi.action.category.MOBABILITY_FINISH,
        animationID    = 718,
        actionID       = xi.mobSkill.CALL_BEAST,
        messageID      = xi.msg.basic.USES,
        param          = 0,
    },

    [xi.job.DRG] =
    {
        finishCategory = xi.action.category.MOBABILITY_FINISH,
        animationID    = 438,
        actionID       = xi.mobSkill.CALL_WYVERN_1,
        messageID      = xi.msg.basic.USES,
        param          = 0,
    },

    -- The PUP mobskill has no action message, so the job ability is used.
    [xi.job.PUP] =
    {
        finishCategory = xi.action.category.JOBABILITY_FINISH,
        animationID    = 83,
        actionID       = xi.jobAbility.ACTIVATE,
        messageID      = xi.msg.basic.USES_JA,
        param          = 0,
    },
}

-- Action packet fields for a pet call. A job packet is only chosen for an
-- instant call with no explicit override; everything else falls back to the
-- generic two-hour packet, whose fields the caller may override.
--
-- NOTE: the generic actionID reads params.action.messageID, not actionID.
-- Preserved for parity; it looks like an upstream copy-paste slip.
xi.mob.callPetActionParams = function(callPetJob, inactiveTime, actionOverride)
    if inactiveTime == 0 and not actionOverride then
        local jobAction = callPetJobActions[callPetJob]

        if jobAction then
            return jobAction
        end
    end

    local override = actionOverride or {}

    return
    {
        finishCategory = override.finishCategory or 11,
        actionID       = override.messageID or 307,
        animationID    = override.animationID or 439,
        messageID      = override.messageID or 0,
        param          = override.param or 0,
    }
end

-- Animation timings below a second bug out, so they collapse to an instant call.
xi.mob.callPetInactiveTime = function(inactiveTime)
    if inactiveTime == nil or inactiveTime < 1000 then
        return 0
    end

    return inactiveTime
end

-- Normalizes the petIds argument: a single id becomes a one-entry list, and an
-- absent list falls back to the owner's own pet.
xi.mob.callPetIds = function(petIds, ownerPetId)
    if type(petIds) == 'number' then
        return { petIds }
    elseif petIds == nil then
        return ownerPetId and { ownerPetId } or {}
    end

    return petIds
end

-- Calls are refused while the owner is busy unless explicitly ignored, and
-- require at least one pet that is not already spawned.
xi.mob.callPetAdmitted = function(isBusy, ignoreBusy, hasSummonablePet)
    if isBusy and not ignoreBusy then
        return false
    end

    return hasSummonablePet
end

-- Spawn cap defaults to every candidate pet.
xi.mob.callPetMaxSpawns = function(maxSpawns, petCount)
    return maxSpawns or petCount
end

-- Selects the existing, unspawned pets in caller order, capped by maxSpawns.
-- The callPets entity host supplies each candidate's lookup/spawn state.
xi.mob.callPetSpawnIds = function(candidates, maxSpawns)
    local selected = {}
    for _, candidate in ipairs(candidates or {}) do
        if #selected < (maxSpawns or #candidates) and candidate.exists and not candidate.spawned then
            table.insert(selected, candidate.id)
        end
    end

    return selected
end

-- Builds the temporary spawn position around the owner from injected jitter.
xi.mob.callPetSpawnPosition = function(ownerPos, jitterX, jitterZ)
    return {
        x   = ownerPos.x + (jitterX or 0),
        y   = ownerPos.y,
        z   = ownerPos.z + (jitterZ or 0),
        rot = ownerPos.rot,
    }
end

-- Plans the owner-side timer, inactivity, and summon animation decisions.
xi.mob.callPetAnimationPlan = function(inactiveTime, noAnimation, ignoreInactive)
    inactiveTime = inactiveTime or 0
    local delayed = inactiveTime > 0
    return {
        timerDelay      = inactiveTime,
        stunDuration    = delayed and not ignoreInactive and inactiveTime or 0,
        startAnimation  = delayed and not noAnimation,
        stopAnimation   = delayed and not noAnimation,
        injectAction    = not delayed and not noAnimation,
    }
end

xi.mob.callPets = function(mob, petIds, params)
    params = params or {}
    -- params table:
    --      params.dieWithOwner:   will kill pets immediately if owner dies
    --      params.persistOnDeath: pets persist when owner dies/disengages (default: false)
    --      params.superLink:      mob will assist pet (pet will always assist mob)
    --      params.maxSpawns:      stop if this many pets get spawned
    --      params.ignoreBusy:     allow pets to get summoned even if owner is busy, interupting any action it was performing
    --      params.noAnimation:    no animation packet from owner when calling pet
    --      params.inactiveTime:   how long for the call pet to take (owner will be inactive during period)
    --      params.ignoreInactive: summoner does not become inactive while summoning a pet
    --          this implies using summoner start/stop entity animation packet (which most mobs use when calling either pets or additional helpers)
    -- if inactiveTime is zero, the following will determine an action packet to signal the mob is calling a pet
    --      params.callPetJob will map to a particular mobskill action packet
    --      if not, the function will use a generic 2-hour action packet
    --          optionally you can override particular action packet params with params.action.X (see that code below)
    -- NOTE these are not arbitrary choices, but multiple options to emulate retail behavior for any particular owner of pets/helpers
    -- Short-circuit over the busy gate rather than calling callPetAdmitted, so
    -- a busy owner skips the pet lookups entirely as upstream does.
    if xi.combat.behavior.isEntityBusy(mob) and not params.ignoreBusy then
        return false
    end

    -- ensure petIds is always a table so ipairs doesn't fail below
    petIds = xi.mob.callPetIds(petIds, mob:getPet() and mob:getPet():getID())

    -- make sure at least one pet is available to summon
    local canSummonPets = false
    for _, petId in ipairs(petIds) do
        local petToSummon = GetMobByID(petId)
        if
            petToSummon and
            not petToSummon:isSpawned()
        then
            canSummonPets = true
        end
    end

    if not canSummonPets then
        return false
    end

    -- don't allow times so short the animations will bug out
    params.inactiveTime = xi.mob.callPetInactiveTime(params.inactiveTime)

    local actionParams = xi.mob.callPetActionParams(params.callPetJob, params.inactiveTime, params.action)
    local animationPlan = xi.mob.callPetAnimationPlan(params.inactiveTime, params.noAnimation, params.ignoreInactive)

    params.action = params.action or {}

    -- function to execute when pets are actually called (there may be an inactiveTime)
    local callPetFinish = function(mobArg)
        if mobArg:isDead() then
            return
        end

        -- inject action packet to indicate mob is summoning a pet
        if animationPlan.stopAnimation then
            mobArg:entityAnimationPacket(xi.animationString.CAST_SUMMONER_STOP)
        elseif animationPlan.injectAction and actionParams then
            -- Generic 2-hour animation with no message
            mobArg:injectActionPacket(mobArg:getID(), actionParams.finishCategory, actionParams.animationID, 0, 0x18, actionParams.messageID, actionParams.actionID, actionParams.param)
        end

        local spawnPos = mobArg:getSpawnPos()
        local pos = mobArg:getPos()
        params.maxSpawns = xi.mob.callPetMaxSpawns(params.maxSpawns, #petIds)
        local spawnedCount = 0
        for _, petId in ipairs(petIds) do
            local petToSummon = GetMobByID(petId)
            if
                spawnedCount < params.maxSpawns and
                petToSummon and
                not petToSummon:isSpawned()
            then
                spawnedCount = spawnedCount + 1
                -- spawn pet around owner
                local petPos = xi.mob.callPetSpawnPosition(pos, math.random(-2, 2), math.random(-2, 2))
                petToSummon:setSpawn(petPos.x, petPos.y, petPos.z, petPos.rot)
                petToSummon:spawn()
                -- set home to be the owner's home position
                petToSummon:setSpawn(spawnPos.x, spawnPos.y, spawnPos.z, spawnPos.rot)

                local ownerRoamListenerName = fmt('OWNER_ASSIST_{}', petId)
                if params.superLink then
                    mobArg:addListener('ROAM_TICK', ownerRoamListenerName, function(owner)
                        local petToAssist = GetMobByID(petId)
                        local assistTarg  = petToAssist and petToAssist:getTarget() or nil
                        if assistTarg then
                            owner:updateEnmity(assistTarg)
                        end
                    end)
                end

                -- so they die at the same time
                -- even without this parameter, if the owner is dead and the pet is roaming, it will die
                if params.dieWithOwner then
                    local listenerName = fmt('OWNER_DEATH_{}', petId)
                    mobArg:addListener('DEATH', listenerName, function(owner)
                        local petToKill = GetMobByID(petId)
                        if petToKill and petToKill:isSpawned() then
                            petToKill:setHP(0)
                        end

                        owner:removeListener(listenerName)
                    end)
                end

                -- make pet assist with a slight delay to allow spawn to complete so animations don't get bugged
                local ownerID = mobArg:getID()
                petToSummon:stun(500)
                if petToSummon ~= mobArg:getPet() then
                    local persistOnDeath = params.persistOnDeath or false
                    if persistOnDeath then
                        petToSummon:addListener('ROAM_TICK', 'ASSIST_OWNER', function(petArg)
                            local owner = GetMobByID(ownerID)
                            if not owner then
                                return
                            end

                            local newTarget = owner:getTarget() or nil
                            if newTarget then
                                petArg:updateEnmity(newTarget)
                                return
                            end

                            if owner:isAlive() and not petArg:hasFollowTarget() then
                                petArg:follow(owner, xi.followType.ROAM)
                                return
                            end
                        end)
                    else
                        petToSummon:addListener('ROAM_TICK', 'ASSIST_OWNER', function(petArg)
                            local owner = GetMobByID(ownerID)
                            if not owner then
                                return
                            end

                            local newTarget = owner:getTarget() or nil
                            if newTarget then
                                petArg:updateEnmity(newTarget)
                                return
                            end

                            if owner:isDead() then
                                petArg:setHP(0)
                                return
                            end

                            if not petArg:hasFollowTarget() then
                                petArg:follow(owner, xi.followType.ROAM)
                                return
                            end
                        end)
                    end

                    -- so we don't wait for the next roam tick (pet assists as soon as :stun is complete)
                    petToSummon:queue(0, function(petArg)
                        petArg:triggerListener('ROAM_TICK', petArg)
                    end)
                end

                -- cleanup any listeners related to this pet when it dies
                -- (:removeListener quietly exits if the listener doesn't exist)
                petToSummon:addListener('DESPAWN', 'PET_LISTENER_CLEANUP', function(petArg)
                    local owner = GetMobByID(ownerID)
                    if owner then
                        owner:removeListener(ownerRoamListenerName)
                    end

                    petArg:removeListener('ASSIST_OWNER')
                    petArg:removeListener('PET_LISTENER_CLEANUP')
                end)
            end
        end
    end

    if animationPlan.stunDuration > 0 then
        -- put owner into inactive state until the timer fires
        mob:stun(animationPlan.stunDuration)
    end

    if animationPlan.startAnimation then
        mob:entityAnimationPacket(xi.animationString.CAST_SUMMONER_START)
    end

    -- regardless, call the anonymous function from above in params.inactiveTime ms (possibly zero)
    -- note that timers cause xi.combat.behavior.isEntityBusy to return true, and so does mob:stun(X)
    mob:timer(animationPlan.timerDelay, callPetFinish)

    return true
end
