-----------------------------------
-- Ship bound for [Mhaura/Selbina] Pirates helpers
-- NOTE: Careful with queues as they don't resolve until a zone wakes from sleep, potentially having mismatched timing. Timers are fine.
-----------------------------------
xi = xi or {}
xi.pirates = xi.pirates or {}
-----------------------------------

local actions =
{
    ARRIVING        = 0,
    ARRIVE          = 1,
    PIRATES_ARRIVE  = 2,
    MOBS_SPAWN      = 3,
    PIRATES_RETREAT = 4,
    DEPART          = 5,
    DEPARTING       = 6,
}

-- Times are minutes after midnight for first cycle. Cycle is 480 minutes.
local piratesSchedule =
{
    { endTime = utils.timeStringToMinutes('01:10'), action = actions.ARRIVING        },
    { endTime = utils.timeStringToMinutes('01:30'), action = actions.ARRIVE          },
    { endTime = utils.timeStringToMinutes('01:32'), action = actions.PIRATES_ARRIVE  },
    { endTime = utils.timeStringToMinutes('01:34'), action = actions.MOBS_SPAWN      },
    { endTime = utils.timeStringToMinutes('04:20'), action = actions.PIRATES_RETREAT },
    { endTime = utils.timeStringToMinutes('04:27'), action = actions.DEPART          },
    { endTime = utils.timeStringToMinutes('04:48'), action = actions.DEPARTING       },
}

local piratesData =
{
    -- Pirate ship is on left side of boat.
    [xi.zone.SHIP_BOUND_FOR_SELBINA_PIRATES] =
    {
        {
            startPos    = { x = -33.601, y = -7.16, z = 13.37, rotation = 0 },
            standingPos = { x = -21.900, y = -7.16, z = 10.46, rotation = 0 },
        },
        {
            startPos    = { x = -29.728, y = -7.16, z =  1.30, rotation = 0 },
            standingPos = { x = -21.900, y = -7.16, z =  6.59, rotation = 0 },
        },
        {
            startPos    = { x = -29.602, y = -7.16, z = -2.47, rotation = 0 },
            standingPos = { x = -21.900, y = -7.16, z =  2.10, rotation = 0 },
        },
    },
    -- Pirate ship is on right side of boat.
    [xi.zone.SHIP_BOUND_FOR_MHAURA_PIRATES] =
    {
        {
            startPos    = { x = 33.601, y = -7.16, z = 13.37, rotation = 128 },
            standingPos = { x = 21.900, y = -7.16, z = 10.46, rotation = 128 },
        },
        {
            startPos    = { x = 29.728, y = -7.16, z =  1.30, rotation = 128 },
            standingPos = { x = 21.900, y = -7.16, z =  6.59, rotation = 128 },
        },
        {
            startPos    = { x = 29.602, y = -7.16, z = -2.47, rotation = 128 },
            standingPos = { x = 21.900, y = -7.16, z =  2.10, rotation = 128 },
        },
    },
}

-- Pure halves of the pirates encounter, extracted so the schedule and the two
-- rolls are testable without a zone, NPC, or RNG.

xi.pirates.actions      = actions
xi.pirates.cycleMinutes = 480

-- The encounter schedule in cycle order: each entry ends at endTime minutes
-- after midnight and drives the periodic trigger for its action.
xi.pirates.schedule = function()
    return piratesSchedule
end

-- Route-specific pirate start and standing positions in pirate order.
xi.pirates.positions = function(zoneId)
    return piratesData[zoneId] or {}
end

-- The middle pirate is the one that may wear a verm cloak.
xi.pirates.vermCloakPirateIndex = 2

-- Model ids for the middle pirate's body slot.
xi.pirates.vermCloakModelId  = 47
xi.pirates.defaultBodyModelId = 8195

-- A 1..100 roll wears the verm cloak on a 10 or under, which is what makes the
-- ride NM-eligible.
xi.pirates.vermCloakRollPassed = function(roll)
    return roll <= 10
end

xi.pirates.bodyModelId = function(hasVermCloak)
    return hasVermCloak and xi.pirates.vermCloakModelId or xi.pirates.defaultBodyModelId
end

-- A ride is NM-eligible while its nmCanSpawn flag is still set; it is cleared
-- to 0 once the NM is up.
xi.pirates.nmEligible = function(nmCanSpawn)
    return nmCanSpawn == 1
end

-- On an eligible ride a 1..100 roll raises the NM at 75 or under.
xi.pirates.nmRollPassed = function(roll)
    return roll <= 75
end

-- Which mob the MOBS_SPAWN step raises: the NM only on an eligible ride whose
-- roll passes, otherwise the placeholder Wight.
--
-- Callers sharing the RNG should short-circuit over nmEligible and nmRollPassed
-- instead, because upstream only rolls once the ride is eligible.
xi.pirates.mobsSpawnSelection = function(nmCanSpawn, roll)
    if
        xi.pirates.nmEligible(nmCanSpawn) and
        xi.pirates.nmRollPassed(roll)
    then
        return 'nm'
    end

    return 'wight'
end

-- The zone-wide work for one periodic action. Every pirate NPC receives the
-- same trigger, so a repeated action makes no changes. Host code still owns
-- mob lookup and the successful-NM-spawn guard around clearing nmCanSpawn.
xi.pirates.zoneStatePlan = function(currentAction, action, nmCanSpawn, roll)
    if currentAction == action then
        return { setAction = false }
    end

    local plan =
    {
        setAction          = true,
        action             = action,
        clearPirates       = false,
        respawnCrossbones  = false,
        spawn              = nil,
        clearNMCanSpawn    = false,
    }

    if action == actions.MOBS_SPAWN then
        plan.clearPirates      = true
        plan.respawnCrossbones = true
        plan.spawn             = xi.pirates.mobsSpawnSelection(nmCanSpawn, roll)
        plan.clearNMCanSpawn   = plan.spawn == 'nm'
    elseif action == actions.PIRATES_RETREAT then
        plan.clearPirates = true
    end

    return plan
end

-- This ride's NM: Blackbeard sails the Selbina route, Silverhook the Mhaura route.
xi.pirates.nmIsBlackbeard = function(zoneId)
    return zoneId == xi.zone.SHIP_BOUND_FOR_SELBINA_PIRATES
end

local function getNMId(zoneId)
    if xi.pirates.nmIsBlackbeard(zoneId) then
        return zones[zoneId].mob.BLACKBEARD
    end

    return zones[zoneId].mob.SILVERHOOK
end

-- Clear the deck of pirate mobs: disable respawns, despawn idle ones, and leave any still in combat to be finished off (they won't respawn).
-- Used at retreat and before a fresh ride spawns
local function clearPirates(zoneId)
    local ID         = zones[zoneId]
    local mobIdTable = { ID.mob.SHIP_WIGHT, getNMId(zoneId) }
    for _, mobId in ipairs(ID.mob.CROSSBONES) do
        table.insert(mobIdTable, mobId)
    end

    for _, mobId in ipairs(mobIdTable) do
        local mob = GetMobByID(mobId)
        if mob then
            mob:setRespawnTime(0) -- Stop the waves / Cancel any pending respawn.
            if mob:isSpawned() and not mob:isEngaged() then
                DespawnMob(mobId) -- Engaged mobs stay until killed, then won't return.
            end
        end
    end
end

-- The pure state transition for one summon-animation timer tick. The host
-- owns entity reads, local-var writes, position/animation calls, and timer
-- scheduling; randomized durations are supplied only for the selected branch.
xi.pirates.summonAnimationPlan = function(hidden, followingPath, initialState, currentTime, summonStartTime, summonEndTime, offset, startDuration, nextStartDelay)
    local plan =
    {
        clearInitialState   = false,
        rotateToBoat        = false,
        setSummonStartTime  = false,
        summonStartTime     = 0,
        setSummonEndTime    = false,
        summonEndTime       = 0,
        startAnimation      = false,
        stopAnimation       = false,
        hide                = false,
        scheduleNext        = false,
    }

    if hidden then
        return plan
    end

    if followingPath then
        plan.scheduleNext = true
        return plan
    end

    if initialState then
        plan.clearInitialState  = true
        plan.rotateToBoat       = true
        summonStartTime         = currentTime + (offset - 1) * 2
        plan.setSummonStartTime = true
        plan.summonStartTime    = summonStartTime
    end

    if summonStartTime ~= 0 and summonStartTime <= currentTime then
        summonStartTime         = 0
        summonEndTime           = currentTime + startDuration
        plan.setSummonStartTime = true
        plan.summonStartTime    = summonStartTime
        plan.setSummonEndTime   = true
        plan.summonEndTime      = summonEndTime
        plan.startAnimation     = true
    end

    if summonEndTime ~= 0 and summonEndTime <= currentTime then
        summonStartTime         = currentTime + nextStartDelay
        summonEndTime           = 0
        plan.setSummonStartTime = true
        plan.summonStartTime    = summonStartTime
        plan.setSummonEndTime   = true
        plan.summonEndTime      = summonEndTime
        plan.stopAnimation      = true
    end

    if summonEndTime == 0 and summonStartTime == 0 then
        plan.hide = true
    else
        plan.scheduleNext = true
    end

    return plan
end

-- Calls itself via timer until the npc is hidden.
local function summonAnimations(npc, rotation, offset)
    if npc:getStatus() == xi.status.DISAPPEAR then
        return
    end

    local followingPath = npc:isFollowingPath()
    local initialState  = false
    local currentTime   = 0
    local summonStartTime = 0
    local summonEndTime   = 0
    local startDuration   = 0
    local nextStartDelay  = 0

    if not followingPath then
        currentTime     = GetSystemTime()
        initialState    = npc:getLocalVar('initialNpcState') == 1
        summonStartTime = npc:getLocalVar('summonStartTime')
        summonEndTime   = npc:getLocalVar('summonEndTime')

        local plannedStartTime = summonStartTime
        if initialState then
            plannedStartTime = currentTime + (offset - 1) * 2
        end

        if plannedStartTime ~= 0 and plannedStartTime <= currentTime then
            startDuration = math.random(1, 2)
        elseif summonEndTime ~= 0 and summonEndTime <= currentTime then
            nextStartDelay = math.random(4 + offset, 10)
        end
    end

    local plan = xi.pirates.summonAnimationPlan(false, followingPath, initialState, currentTime, summonStartTime, summonEndTime, offset, startDuration, nextStartDelay)

    if plan.clearInitialState then
        npc:setLocalVar('initialNpcState', 0)
    end

    if plan.rotateToBoat then
        local pos = npc:getPos()
        -- rotate to face the player boat
        npc:setPos(pos.x, pos.y, pos.z, rotation)
    end

    if plan.setSummonStartTime then
        npc:setLocalVar('summonStartTime', plan.summonStartTime)
    end

    if plan.setSummonEndTime then
        npc:setLocalVar('summonEndTime', plan.summonEndTime)
    end

    if plan.startAnimation then
        npc:entityAnimationPacket(xi.animationString.CAST_SUMMONER_START)
    end

    if plan.stopAnimation then
        npc:entityAnimationPacket(xi.animationString.CAST_SUMMONER_STOP)
    end

    -- No more animations and npc is done pathing.
    if plan.hide then
        npc:setStatus(xi.status.DISAPPEAR)
    end

    if plan.scheduleNext then
        -- check again in 1.2s (pirates summon animation can last from 1s to 2s)
        npc:timer(1200, function(npcArg)
            summonAnimations(npcArg, rotation, offset)
        end)
    end
end

xi.pirates.setupPirateNPCSchedule = function(npc)
    npc:initNpcAi()

    -- Create triggers for every stage of the encounter on each Pirate NPC.
    for _, eventData in ipairs(xi.pirates.schedule()) do
        npc:addPeriodicTrigger(eventData.action, xi.pirates.cycleMinutes, eventData.endTime)
    end
end

-- Called on every NPC periodic trigger, which is mapped 1-1 to the schedule table, with triggerId == action
xi.pirates.pirateNPCTimeTrigger = function(npc, triggerId, zoneKey)
    local pirateZone = npc:getZone()
    if not pirateZone then
        return
    end

    local pirateNPCs = zones[pirateZone:getID()].npc.PIRATES
    local pirateIdx  = 0

    for i, npcId in ipairs(pirateNPCs) do
        if npcId == npc:getID() then
            pirateIdx = i
            break
        end
    end

    local pirateData = piratesData[zoneKey][pirateIdx]
    if not pirateData then
        return
    end

    -- Pirates appear and run to position
    if triggerId == actions.PIRATES_ARRIVE then
        if pirateIdx == xi.pirates.vermCloakPirateIndex then
            -- middle pirate has chance to wear a verm cloak, which then means the pirate encounter _might_ have the NM spawn
            local hasVermCloak = xi.pirates.vermCloakRollPassed(math.random(1, 100))
            npc:setModelId(xi.pirates.bodyModelId(hasVermCloak), xi.slot.BODY)
            pirateZone:setLocalVar('nmCanSpawn', hasVermCloak and 1 or 0) -- 1 = NM still eligible; cleared to 0 once it spawns
        end

        npc:setPos(pirateData.startPos)
        npc:setStatus(xi.status.NORMAL)
        npc:clearPath()
        npc:pathTo(pirateData.standingPos.x, pirateData.standingPos.y, pirateData.standingPos.z, xi.path.flag.RUN + xi.path.flag.WALLHACK)

        -- Indicates we need to rotate NPC after pathing completes
        npc:setLocalVar('initialNpcState', 1)
        summonAnimations(npc, pirateData.standingPos.rotation, pirateIdx)

    -- Retreat.
    elseif triggerId == actions.PIRATES_RETREAT then
        local summonEndTime = npc:getLocalVar('summonEndTime')
        -- No more animations will happen and recursive function self destructs
        npc:setLocalVar('summonStartTime', 0)
        npc:setLocalVar('summonEndTime', 0)
        if summonEndTime > 0 then
            npc:entityAnimationPacket(xi.animationString.CAST_SUMMONER_STOP)
        end

        npc:pathTo(pirateData.startPos.x, pirateData.startPos.y, pirateData.startPos.z, xi.path.flag.RUN + xi.path.flag.WALLHACK)

    -- Just in case summonAnimations didn't set status
    elseif triggerId == actions.DEPART then
        npc:clearPath()
        npc:setStatus(xi.status.DISAPPEAR)
    end

    xi.pirates.zoneStateChange(pirateZone, triggerId)
end

xi.pirates.zoneStateChange = function(zone, action)
    -- change the zone's state once per action cycle (this function is called by each NPC)
    local currentAction = zone:getLocalVar('currPiratesAction')
    if currentAction == action then
        return
    end

    zone:setLocalVar('currPiratesAction', action)

    local zoneId     = zone:getID()
    local ID         = zones[zoneId]
    local nmCanSpawn = 0
    local roll       = 0
    if action == actions.MOBS_SPAWN then
        nmCanSpawn = zone:getLocalVar('nmCanSpawn')
        if xi.pirates.nmEligible(nmCanSpawn) then
            roll = math.random(1, 100)
        end
    end

    local plan = xi.pirates.zoneStatePlan(currentAction, action, nmCanSpawn, roll)

    if plan.clearPirates then
        -- clear any mobs lingering from a previous ride before summoning fresh ones
        clearPirates(zoneId)
    end

    if plan.respawnCrossbones then
        -- the skeletons the pirate NPCs are "summoning" onto the deck
        for _, mobId in ipairs(ID.mob.CROSSBONES) do
            local crossbones = GetMobByID(mobId)
            if crossbones then
                crossbones:setRespawnTime(1)
            end
        end
    end

    if plan.spawn == 'nm' then
        -- HQ ride, 75%: NM appears from the start
        local nm = GetMobByID(getNMId(zoneId))
        if nm then
            nm:setRespawnTime(1)
            if plan.clearNMCanSpawn then
                zone:setLocalVar('nmCanSpawn', 0) -- NM is up; no longer eligible to spawn
            end
        end
    elseif plan.spawn == 'wight' then
        -- normal ride, or the 25% placeholder Wight on an HQ ride
        local wight = GetMobByID(ID.mob.SHIP_WIGHT)
        if wight then
            wight:setRespawnTime(1)
        end
    end
end
