require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.qutrub = xi.mix.qutrub or {}

xi.mix.qutrub.swapPlan = function(now, swapTime, animationSub)
    if swapTime > 0 and now > swapTime then
        if animationSub == 1 then
            return { animationSub = 2, swapTime = now + 60 }
        elseif animationSub == 2 then
            return { animationSub = 1, swapTime = now + 60 }
        end
    end

    return nil
end

xi.mix.qutrub.breakPlan = function(now, animationSub, breakChance, roll)
    if roll <= breakChance then
        if animationSub == 0 or animationSub == 4 then
            return { animationSub = 1, swapTime = now + 60 }
        elseif animationSub == 2 then
            return { animationSub = 3, swapTime = 0 }
        end
    end

    return nil
end

-- AnimationSub for Qutrub
-- 0 = main weapon out, sub weapon sheathed
-- 1 = main weapon broken, sub weapon sheathed
-- 2 = main weapon broken, sub weapon out
-- 3 = both weapons broken
-- 4 = main weapon out, sub weapon sheathed
g_mixins.families.qutrub = function(qutrubMob)
    -- set default 10% chance to break weapon on critical hit taken
    -- this can be overridden in onMobSpawn

    qutrubMob:addListener('SPAWN', 'QUTRUB_SPAWN', function(mob)
        mob:setLocalVar('qutrubBreakChance', 10)
    end)

    -- when a qutrub's weapon has been broken it will switch between using its second

    qutrubMob:addListener('COMBAT_TICK', 'QUTRUB_COMBAT_TICK', function(mob)
        local plan = xi.mix.qutrub.swapPlan(GetSystemTime(), mob:getLocalVar('swapTime'), mob:getAnimationSub())

        if plan then
            mob:setAnimationSub(plan.animationSub)
            mob:setLocalVar('swapTime', plan.swapTime)
        end
    end)

    -- chance to break weapon when taking a critical hit

    qutrubMob:addListener('CRITICAL_TAKE', 'QUTRUB_CRITICAL_TAKE', function(mob)
        local plan = xi.mix.qutrub.breakPlan(
            GetSystemTime(),
            mob:getAnimationSub(),
            mob:getLocalVar('qutrubBreakChance'),
            math.random(1, 100)
        )

        if plan then
            mob:setAnimationSub(plan.animationSub)
            mob:setLocalVar('swapTime', plan.swapTime)
        end
    end)
end

return g_mixins.families.qutrub
