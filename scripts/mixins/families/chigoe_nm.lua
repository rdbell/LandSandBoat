-- Chigoe family mixin (for NMs that do not instantly die from crit/ws/JA)

require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.chigoeNM = xi.mix.chigoeNM or {}

xi.mix.chigoeNM.eventPlan = function(event)
    if event == 'ENGAGE' then
        return { hideName = false, untargetable = false }
    elseif event == 'SPAWN' or event == 'DISENGAGE' then
        return { hideName = true, untargetable = true }
    end
    return nil
end

g_mixins.families.chigoe_nm = function(chigoeMob)
    chigoeMob:addListener('SPAWN', 'CHIGOE_SPAWN', function(mob)
        local plan = xi.mix.chigoeNM.eventPlan('SPAWN')
        mob:hideName(plan.hideName)
        mob:setUntargetable(plan.untargetable)
    end)

    chigoeMob:addListener('ENGAGE', 'CHIGOE_ENGAGE', function(mob, target)
        local plan = xi.mix.chigoeNM.eventPlan('ENGAGE')
        mob:hideName(plan.hideName)
        mob:setUntargetable(plan.untargetable)
    end)

    chigoeMob:addListener('DISENGAGE', 'CHIGOE_DISENGAGE', function(mob, target)
        local plan = xi.mix.chigoeNM.eventPlan('DISENGAGE')
        mob:hideName(plan.hideName)
        mob:setUntargetable(plan.untargetable)
    end)
end

return g_mixins.families.chigoe_nm
