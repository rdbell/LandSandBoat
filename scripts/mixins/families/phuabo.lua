-- Phuabo family mixin

require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

local hiddenPlan = { hidden = true, untargetable = true, animationSub = 5, waitMs = 2000 }
local engagedPlan = { hidden = false, untargetable = false, animationSub = 6, waitMs = 2000 }

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.phuabo = xi.mix.phuabo or {}

xi.mix.phuabo.statePlan = function(isEngaged)
    if isEngaged then
        return engagedPlan
    end

    return hiddenPlan
end

local function applyPlan(mob, plan)
    mob:hideName(plan.hidden)
    mob:setUntargetable(plan.untargetable)
    mob:setAnimationSub(plan.animationSub)
    mob:wait(plan.waitMs)
end

g_mixins.families.phuabo = function(phuaboMob)
    phuaboMob:addListener('SPAWN', 'PHUABO_SPAWN', function(mob)
        applyPlan(mob, xi.mix.phuabo.statePlan(false))
    end)

    phuaboMob:addListener('ENGAGE', 'PHUABO_ENGAGE', function(mob, target)
        applyPlan(mob, xi.mix.phuabo.statePlan(true))
    end)

    phuaboMob:addListener('DISENGAGE', 'PHUABO_DISENGAGE', function(mob, target)
        applyPlan(mob, xi.mix.phuabo.statePlan(false))
    end)
end

return g_mixins.families.phuabo
