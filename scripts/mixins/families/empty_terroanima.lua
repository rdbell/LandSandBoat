-----------------------------------
--  Effect from the item Bottle of Terroanima --
-----------------------------------

require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.emptyTerroanima = xi.mix.emptyTerroanima or {}

xi.mix.emptyTerroanima.terrorPlan = function(terrorStart, terrorDuration, now, isFollowingPath)
    if terrorStart == 0 then
        return nil
    elseif terrorStart + terrorDuration < now then
        return { restore = true }
    elseif terrorStart + terrorDuration > now and not isFollowingPath then
        return { run = true }
    end

    return nil
end

local function doTerrorRun(mob)
    local terrorStart = mob:getLocalVar('EmptyTerror')
    local terrorDuration = mob:getLocalVar('EmptyTerrorDuration')
    local plan = xi.mix.emptyTerroanima.terrorPlan(terrorStart, terrorDuration, GetSystemTime(), mob:isFollowingPath())
    if plan then
        if plan.restore then
            mob:setLocalVar('EmptyTerror', 0)
            mob:setAutoAttackEnabled(true)
            mob:setMobAbilityEnabled(true)
            mob:setMagicCastingEnabled(true)
            mob:setRoamFlags(0)
        elseif plan.run then
            mob:setRoamFlags(256, 512)
            mob:setAutoAttackEnabled(false)
            mob:setMobAbilityEnabled(false)
            mob:setMagicCastingEnabled(false)
            local pos = mob:getPos()
            mob:pathTo(pos.x + math.random(-5, 5), pos.y, pos.z + math.random(-5, 5), 9) -- Pathflags = 9 (xi.pathflag.run, xi.pathflag.scripted)
        end
    end
end

g_mixins.families.empty_terroanima = function(emptyMob)
    emptyMob:addListener('ROAM_TICK', 'EMPTY_TERROANIMA_RTICK', function(mob)
        doTerrorRun(mob)
    end)

    emptyMob:addListener('COMBAT_TICK', 'EMPTY_TERROANIMA_CTICK', function(mob)
        doTerrorRun(mob)
    end)

    emptyMob:addListener('DEATH', 'EMPTY_TERROANIMA_DEATH', function(mob)
        mob:setRoamFlags(0)
    end)
end

return g_mixins.families.empty_terroanima
