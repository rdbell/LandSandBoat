require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.imp_aggro = xi.mix.imp_aggro or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.imp_aggro.detectionForHour = function(hour)
    if hour >= 18 or hour < 6 then
        return bit.bor(xi.detects.SIGHT, xi.detects.HEARING)
    elseif hour < 18 and hour >= 6 then
        return xi.detects.HEARING
    end
end

xi.mix.imp_aggro.shouldUpdate = function(previousHour, hour)
    return hour ~= previousHour
end

local function updateAggro(mob, hour)
    mob:setLocalVar('hour', hour)

    local detection = xi.mix.imp_aggro.detectionForHour(hour)
    if detection ~= nil then
        mob:setMobMod(xi.mobMod.DETECTION, detection)
    end
end

g_mixins.families.imp_aggro = function(mob)
    mob:addListener('SPAWN', 'IMP_AGGRO_SPAWN', function(imp)
        updateAggro(imp, VanadielHour())
    end)

    mob:addListener('ROAM_TICK', 'IMP_AGGRO_ROAM_TICK', function(imp)
        local hour = VanadielHour()
        if xi.mix.imp_aggro.shouldUpdate(imp:getLocalVar('hour'), hour) then
            updateAggro(imp, hour)
        end
    end)
end

return g_mixins.families.imp_aggro
