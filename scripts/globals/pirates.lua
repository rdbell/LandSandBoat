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

-- Calls itself via timer until the npc is hidden.
local function summonAnimations(npc, rotation, offset)
    if npc:getStatus() == xi.status.DISAPPEAR then
        return
    end

    if not npc:isFollowingPath() then
        local pos         = npc:getPos()
        local currentTime = GetSystemTime()

        if npc:getLocalVar('initialNpcState') == 1 then
            npc:setLocalVar('initialNpcState', 0)
            -- rotate to face the player boat
            npc:setPos(pos.x, pos.y, pos.z, rotation)
            -- first summoning rotation happens in order of NPC ID
            npc:setLocalVar('summonStartTime', currentTime + (offset - 1) * 2)
        end

        local summonStartTime = npc:getLocalVar('summonStartTime')
        if summonStartTime ~= 0 and summonStartTime <= currentTime then
            npc:setLocalVar('summonStartTime', 0)
            npc:setLocalVar('summonEndTime', currentTime + math.random(1, 2))

            npc:entityAnimationPacket(xi.animationString.CAST_SUMMONER_START)
        end

        local summonEndTime = npc:getLocalVar('summonEndTime')
        if summonEndTime ~= 0 and summonEndTime <= currentTime then
            npc:setLocalVar('summonStartTime', currentTime + math.random(4 + offset, 10))
            npc:setLocalVar('summonEndTime', 0)

            npc:entityAnimationPacket(xi.animationString.CAST_SUMMONER_STOP)
        end

        -- No more animations and npc is done pathing
        if summonEndTime == 0 and summonStartTime == 0 then
            npc:setStatus(xi.status.DISAPPEAR)
        end
    end

    if npc:getStatus() == xi.status.DISAPPEAR then
        return
    end

    -- check again in 1.2s (pirates summon animation can last from 1s to 2s)
    npc:timer(1200, function(npcArg)
        summonAnimations(npcArg, rotation, offset)
    end)
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
    if zone:getLocalVar('currPiratesAction') == action then
        return
    end

    zone:setLocalVar('currPiratesAction', action)

    local zoneId = zone:getID()
    local ID     = zones[zoneId]

    if action == actions.MOBS_SPAWN then
        -- clear any mobs lingering from a previous ride before summoning fresh ones
        clearPirates(zoneId)

        -- the skeletons the pirate NPCs are "summoning" onto the deck
        for _, mobId in ipairs(ID.mob.CROSSBONES) do
            local crossbones = GetMobByID(mobId)
            if crossbones then
                crossbones:setRespawnTime(1)
            end
        end

        -- Short-circuit over the primitives: upstream only rolls for the NM once
        -- the ride is eligible.
        if
            xi.pirates.nmEligible(zone:getLocalVar('nmCanSpawn')) and
            xi.pirates.nmRollPassed(math.random(1, 100))
        then
            -- HQ ride, 75%: NM appears from the start
            local nm = GetMobByID(getNMId(zoneId))
            if nm then
                nm:setRespawnTime(1)
                zone:setLocalVar('nmCanSpawn', 0) -- NM is up; no longer eligible to spawn
            end
        else
            -- normal ride, or the 25% placeholder Wight on an HQ ride
            local wight = GetMobByID(ID.mob.SHIP_WIGHT)
            if wight then
                wight:setRespawnTime(1)
            end
        end
    elseif action == actions.PIRATES_RETREAT then
        clearPirates(zoneId)
    end
end
