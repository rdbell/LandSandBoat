require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.chariot = xi.mix.chariot or {}

xi.mix.chariot.engagePlan = function(now, turnDelay)
    return { turnTime = now, turnDelay = turnDelay }
end

xi.mix.chariot.turnPlan = function(now, turnTime, turnDelay)
    if now >= turnTime then
        return { turnTime = now + turnDelay }
    end
    return nil
end

g_mixins.families.chariot = function(chariotMob)
    chariotMob:addListener('SPAWN', 'CHARIOT_SPAWN', function(mob)
        mob:setBehavior(bit.bor(mob:getBehavior(), xi.behavior.NO_TURN))
    end)

    chariotMob:addListener('ENGAGE', 'CHARIOT_ENGAGE', function(mob)
        local plan = xi.mix.chariot.engagePlan(GetSystemTime(), math.random(10, 30))
        mob:setLocalVar('turnTime', plan.turnTime)
        mob:setLocalVar('turnDelay', plan.turnDelay)
    end)

    chariotMob:addListener('COMBAT_TICK', 'CHARIOT_COMBAT', function(mob)
        local time = GetSystemTime()

        if time >= mob:getLocalVar('turnTime') then
            local plan = xi.mix.chariot.turnPlan(time, mob:getLocalVar('turnTime'), math.random(10, 30))
            -- mob:face() -- We lack a lua function for c++ FaceTarget. TODO: code it.

            mob:setLocalVar('turnTime', plan.turnTime)
        end
    end)
end

return g_mixins.families.chariot
