require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.imp = xi.mix.imp or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.imp.shouldBreakHorn = function(random, animationSub)
    return random <= 20 and animationSub == 4
end

xi.mix.imp.restoreDelay = function(random)
    if random <= 2 then
        return 60
    end

    return 25 + (math.ceil(random / 5) * 5)
end

xi.mix.imp.criticalPlan = function(random, animationSub, hornDisabled)
    if not xi.mix.imp.shouldBreakHorn(random, animationSub) then
        return nil
    end

    local plan = { animationSub = 5 }
    if hornDisabled ~= 5 then
        plan.restoreDelayMs = xi.mix.imp.restoreDelay(random) * 1000
    end
    return plan
end

xi.mix.imp.shouldRestoreHorn = function(isAlive)
    return isAlive
end

g_mixins.families.imp = function(impMob)
    -- 20% chance to break horn on critical hit
    -- Reacquires horn in stages of 30, 35, 40, 45, and a very rare 60 seconds
    impMob:addListener('CRITICAL_TAKE', 'IMP_CRITICAL_TAKE', function(mob)
        local random = math.random(1, 100)

        local plan = xi.mix.imp.criticalPlan(random, mob:getAnimationSub(), mob:getLocalVar('hornDisabled'))
        if plan then
            mob:setAnimationSub(plan.animationSub)
            if plan.restoreDelayMs then
                mob:timer(plan.restoreDelayMs, function(mobArg)
                    if xi.mix.imp.shouldRestoreHorn(mobArg:isAlive()) then
                        mobArg:setAnimationSub(4)
                    end
                end)
            end
        end
    end)
end

return g_mixins.families.imp
