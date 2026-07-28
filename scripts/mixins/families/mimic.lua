require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.mimic = xi.mix.mimic or {}

xi.mix.mimic.drawInPlan = function(hasTarget, distance, meleeRange)
    if hasTarget and distance >= meleeRange then
        return { offset = meleeRange - 0.2 }
    end
    return nil
end

g_mixins.families.mimic = function(mimicMob)
    mimicMob:addListener('COMBAT_TICK', 'DRAW_IN_CHECK', function(mob)
        local target = mob:getTarget()
        local plan = xi.mix.mimic.drawInPlan(target ~= nil, target and mob:checkDistance(target) or 0, target and mob:getMeleeRange(target) or 0)
        if plan then
            local drawInTable =
            {
                conditions =
                {
                    true,
                },
                position = mob:getPos(),
                offset = plan.offset, -- TODO: does this change by target size?
            }
            utils.drawIn(target, drawInTable)
        end
    end)
end

return g_mixins.families.mimic
