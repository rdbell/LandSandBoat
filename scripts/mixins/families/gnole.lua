-- Gnole family mixin

require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.gnole = xi.mix.gnole or {}

xi.mix.gnole.spawnPlan = function(now)
    return { transformTime = now }
end

xi.mix.gnole.transformPlan = function(now, transformTime, animationSub, timeThreshold)
    if now - transformTime < timeThreshold then
        return nil
    end
    return { animationSub = (animationSub + 1) % 2, transformTime = now }
end

local function attemptTransform(mob, timeThreshold)
    local transformTime = mob:getLocalVar('transformTime')
    local currentTime = GetSystemTime()
    local plan = xi.mix.gnole.transformPlan(currentTime, transformTime, mob:getAnimationSub(), timeThreshold)
    if plan then
        mob:setAnimationSub(plan.animationSub)
        mob:setLocalVar('transformTime', plan.transformTime)
    end
end

g_mixins.families.gnole = function(gnoleMob)
    gnoleMob:addListener('SPAWN', 'GNOLE_SPAWN', function(mob)
        local plan = xi.mix.gnole.spawnPlan(GetSystemTime())
        mob:setLocalVar('transformTime', plan.transformTime)
    end)

    gnoleMob:addListener('ROAM_TICK', 'GNOLE_ROAM', function(mob)
        attemptTransform(mob, 300)
    end)

    gnoleMob:addListener('COMBAT_TICK', 'GNOLE_COMBAT', function(mob)
        attemptTransform(mob, 60)
    end)
end

return g_mixins.families.gnole
