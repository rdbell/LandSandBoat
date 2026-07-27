--[[
https://ffxiclopedia.fandom.com/wiki/Category:Crawlers

Eruca mobs can optionally be modified by calling xi.mix.eruca.config(mob, params) from within onMobSpawn.

params is a table that can contain the following keys:
    sleepHour : changes hour at which eruca crawlers naturally fall asleep (default: 18)
    wakeHour  : changes hour at which eruca crawlers naturally wake (default: 6)

Example:

xi.mix.eruca.config(mob, {
    sleepHour = 20,
    wakeHour = 4,
})

--]]
require('scripts/globals/mixins')
require('scripts/globals/magic')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.eruca = xi.mix.eruca or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.eruca.resleepTime = function(now)
    return now + 120
end

xi.mix.eruca.roamAction = function(params)
    params = params or {}

    local subAnimation = params.subAnimation or 0
    local currentHour = params.currentHour or 0
    local sleepHour = params.sleepHour or 0
    local wakeHour = params.wakeHour or 0

    if
        subAnimation == 0 and
        (currentHour >= sleepHour or currentHour < wakeHour) and
        not params.engaged
    then
        local resleepTime = params.resleepTime or 0
        if resleepTime ~= 0 and (params.distanceFromSpawn or 0) > 25 then
            return 'resetResleep'
        elseif resleepTime <= (params.now or 0) then
            return 'sleep'
        end
    elseif
        subAnimation == 1 and
        currentHour < sleepHour and
        currentHour >= wakeHour
    then
        return 'wake'
    end

    return 'none'
end

xi.mix.eruca.regainAction = function(isFireDay, currentRegain)
    if isFireDay and currentRegain == 0 then
        return 30
    elseif not isFireDay and currentRegain ~= 0 then
        return 0
    end
end

local function bedTime(mob)
    mob:setAnimationSub(mob:getAnimationSub() + 1)
    mob:setMobMod(xi.mobMod.NO_MOVE, 1)
    mob:setMobMod(xi.mobMod.NO_AGGRO, 1)
    mob:setMobMod(xi.mobMod.NO_LINK, 1)
    mob:setMagicCastingEnabled(false)
    mob:setLocalVar('ResleepTime', 0)
end

local function wakeUp(mob)
    mob:setAnimationSub(mob:getAnimationSub() - 1)
    mob:setMobMod(xi.mobMod.NO_MOVE, 0)
    mob:setMobMod(xi.mobMod.NO_AGGRO, 0)
    mob:setMobMod(xi.mobMod.NO_LINK, 0)
    mob:setMagicCastingEnabled(true)
    mob:setLocalVar('ResleepTime', 0)
end

xi.mix.eruca.config = function(mob, params)
    if params.sleepHour and type(params.sleepHour) == 'number' then
        mob:setLocalVar('[eruca]sleepHour', params.sleepHour)
    end

    if params.wakeHour and type(params.wakeHour) == 'number' then
        mob:setLocalVar('[eruca]wakeHour', params.wakeHour)
    end
end

g_mixins.families.eruca = function(erucaMob)
    -- these defaults can be overwritten by using xi.mix.eruca.config() in onMobSpawn.  sleepHour must be > wakeHour to function properly.
    erucaMob:addListener('SPAWN', 'ERUCA_SPAWN', function(mob)
        mob:setLocalVar('[eruca]sleepHour', 18)
        mob:setLocalVar('[eruca]wakeHour', 6)
    end)

    erucaMob:addListener('ROAM_TICK', 'ERUCA_ROAM_TICK', function(mob)
        local currentHour = VanadielHour()
        local sleepHour = mob:getLocalVar('[eruca]sleepHour')
        local subAnimation = mob:getAnimationSub()
        local wakeHour = mob:getLocalVar('[eruca]wakeHour')
        local resleepTime = mob:getLocalVar('ResleepTime')
        local engaged = false
        local distanceFromSpawn = 0
        local now = 0

        if subAnimation == 0 and (currentHour >= sleepHour or currentHour < wakeHour) then
            engaged = mob:isEngaged()
            if not engaged then
                now = GetSystemTime()
                if resleepTime ~= 0 then
                    distanceFromSpawn = mob:checkDistance(mob:getSpawnPos())
                end
            end
        end

        local roamAction = xi.mix.eruca.roamAction({
            subAnimation = subAnimation,
            currentHour = currentHour,
            sleepHour = sleepHour,
            wakeHour = wakeHour,
            engaged = engaged,
            resleepTime = resleepTime,
            distanceFromSpawn = distanceFromSpawn,
            now = now,
        })

        if roamAction == 'resetResleep' then
            mob:setLocalVar('ResleepTime', xi.mix.eruca.resleepTime(now))
        elseif roamAction == 'sleep' then
            bedTime(mob)
        elseif roamAction == 'wake' then
            wakeUp(mob)
        end

        local regain = xi.mix.eruca.regainAction(VanadielDayElement() == xi.element.FIRE, mob:getMod(xi.mod.REGAIN))
        if regain ~= nil then
            mob:setMod(xi.mod.REGAIN, regain)
        end
    end)

    erucaMob:addListener('ENGAGE', 'ERUCA_ENGAGE', function(mob, target)
        if mob:getAnimationSub() == 1 then
            wakeUp(mob)
        end
    end)

    erucaMob:addListener('DISENGAGE', 'ERUCA_DISENGAGE', function(mob)
        mob:setLocalVar('ResleepTime', xi.mix.eruca.resleepTime(GetSystemTime())) -- Eruca crawlers go back to sleep exactly 2 minutes after they were engaged.
    end)
end

return g_mixins.families.eruca
