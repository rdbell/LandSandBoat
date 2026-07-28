require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.ruszor = xi.mix.ruszor or {}

xi.mix.ruszor.combatPlan = function(animationSub, hasStoneskin, iceAbsorb, waterAbsorb)
    if animationSub == 1 and hasStoneskin and iceAbsorb == 0 then
        return { iceAbsorb = 100, waterAbsorb = 0 }
    elseif animationSub == 2 and hasStoneskin and waterAbsorb == 0 then
        return { iceAbsorb = 0, waterAbsorb = 100 }
    elseif animationSub ~= 0 and not hasStoneskin then
        -- Reset on the next combat tick so consecutive moves can preserve form.
        return { animationSub = 0, iceAbsorb = 0, waterAbsorb = 0 }
    end
    return nil
end

g_mixins.families.ruszor = function(ruszorMob)
    ruszorMob:addListener('COMBAT_TICK', 'RUSZOR_AURA', function(mob)
        local animationSub = mob:getAnimationSub()
        local hasEffect    = mob:hasStatusEffect(xi.effect.STONESKIN)
        local plan = xi.mix.ruszor.combatPlan(
            animationSub,
            hasEffect,
            mob:getMod(xi.mod.ICE_ABSORB),
            mob:getMod(xi.mod.WATER_ABSORB))
        if plan then
            if plan.animationSub ~= nil then
                mob:setAnimationSub(plan.animationSub)
            end
            mob:setMod(xi.mod.ICE_ABSORB, plan.iceAbsorb)
            mob:setMod(xi.mod.WATER_ABSORB, plan.waterAbsorb)
        end
    end)
end

return g_mixins.families.ruszor
