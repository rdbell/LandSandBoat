--[[
https://ffxiclopedia.fandom.com/wiki/Ziz

AnimationSub(1) small neck pouch
AnimationSub(2) large neck pouch
AnimationSub(3) sleeping z's
--]]
require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.ziz = xi.mix.ziz or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.ziz.sleepPlan = function(totd, animationSub)
    if totd == xi.time.NIGHT or totd == xi.time.MIDNIGHT then
        if animationSub ~= 3 then
            return { animationSub = 3, aggressive = false, noMove = 1 }
        end
    elseif animationSub ~= 1 then
        return { animationSub = 1, aggressive = true, noMove = 0 }
    end
end

local function sleepDuringNight(mob)
    local aSub = mob:getAnimationSub()
    local totd = VanadielTOTD()

    local plan = xi.mix.ziz.sleepPlan(totd, aSub)
    if plan then
        mob:setAnimationSub(plan.animationSub)
        mob:setAggressive(plan.aggressive)
        mob:setMobMod(xi.mobMod.NO_MOVE, plan.noMove)
    end
end

g_mixins.families.ziz = function(zizMob)
    zizMob:addListener('SPAWN', 'ZIZ_SPAWN', function(mob)
        sleepDuringNight(mob)
    end)

    zizMob:addListener('ROAM_TICK', 'ZIZ_ROAM', function(mob)
        sleepDuringNight(mob)
    end)

    zizMob:addListener('ENGAGE', 'ZIZ_ENGAGE', function(mob, target)
        mob:setAnimationSub(1)
        mob:setAggressive(true)
        mob:setMobMod(xi.mobMod.NO_MOVE, 0)
    end)
end

return g_mixins.families.ziz
