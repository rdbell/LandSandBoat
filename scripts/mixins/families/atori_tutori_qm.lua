require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.atoriTutori = xi.mix.atoriTutori or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.atoriTutori.spawnThreshold = function()
    return 35
end

xi.mix.atoriTutori.engagePlan = function(engaged, targetID)
    if engaged ~= 0 then
        return nil
    end

    return { engaged = targetID }
end

xi.mix.atoriTutori.specialPlan = function(hpp, threshold)
    if hpp >= threshold then
        return nil
    end

    return { threshold = 0, power = 1, duration = 30 }
end

xi.mix.atoriTutori.isTerror = function(effectType)
    return effectType == xi.effect.TERROR
end

g_mixins.families.atori_tutori_qm = function(atoriMob)
    atoriMob:addListener('SPAWN', 'JOB_SPECIAL_SPAWN', function(mob)
        mob:setLocalVar('specialThreshold', xi.mix.atoriTutori.spawnThreshold())
    end)

    atoriMob:addListener('ENGAGE', 'ATORI_ENGAGE', function(mob, target)
        local engaged = mob:getLocalVar('engaged')
        if engaged == 0 then
            local plan = xi.mix.atoriTutori.engagePlan(engaged, target:getID())
            local ID = zones[mob:getZoneID()]
            mob:messageText(mob, ID.text.PROMISE_ME_YOU_WONT_GO_DOWN)
            mob:setLocalVar('engaged', plan.engaged)
        end
    end)

    atoriMob:addListener('COMBAT_TICK', 'ATORI_CTICK', function(mob)
        local plan = xi.mix.atoriTutori.specialPlan(mob:getHPP(), mob:getLocalVar('specialThreshold'))
        if plan then
            local ID = zones[mob:getZoneID()]
            mob:messageText(mob, ID.text.YOU_PACKED_MORE_OF_A_PUNCH)
            xi.mobskills.mobBuffMove(mob, xi.effect.HUNDRED_FISTS, plan.power, 0, plan.duration)
            mob:setLocalVar('specialThreshold', plan.threshold)
        end
    end)

    atoriMob:addListener('DEATH', 'ATORI_DEATH', function(mob, killer)
        local ID = zones[mob:getZoneID()]
        mob:messageText(mob, ID.text.WHATS_THIS_STRANGE_FEELING)
    end)

    atoriMob:addListener('EFFECT_GAIN', 'TERRORIZED', function(mob, effect)
        local ID = zones[mob:getZoneID()]
        if xi.mix.atoriTutori.isTerror(effect:getEffectType()) then
            mob:showText(mob, ID.text.YIKEY_WIKEYS)
        end
    end)

    atoriMob:addListener('EFFECT_LOSE', 'TERRORIZED', function(mob, effect)
        local ID = zones[mob:getZoneID()]
        if xi.mix.atoriTutori.isTerror(effect:getEffectType()) then
            mob:showText(mob, ID.text.WHATS_THE_MATTARU)
        end
    end)
end

return g_mixins.families.atori_tutori_qm
