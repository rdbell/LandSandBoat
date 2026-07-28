-- Chigoe(pet) family mixin

require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.chigoePet = xi.mix.chigoePet or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.chigoePet.spawnCount = function(pool)
    if pool == xi.mobPool.PEALLAIDH then
        return 2
    end

    return 1
end

xi.mix.chigoePet.shouldSpawnCandidate = function(exists, spawned, remaining)
    return exists and not spawned and remaining > 0
end

g_mixins.families.chigoe_pet = function(hostMob)
    local ID = zones[hostMob:getZoneID()]

    hostMob:addListener('WEAPONSKILL_USE', 'MOB_SPAWN_CHIGOE', function(mob, target, skill, tp, action, damage)
        local mobName = mob:getName()

        -- Requires a Chigoe.lua with the chigoe mixin for this to work
        if ID.mob.CHIGOES[mobName] == nil then
            return
        end

        local numChigoesToSpawn = xi.mix.chigoePet.spawnCount(mob:getPool())

        for _, mobID in pairs(ID.mob.CHIGOES[mobName]) do
            local chigoe = GetMobByID(mobID)

            if xi.mix.chigoePet.shouldSpawnCandidate(chigoe ~= nil, chigoe and chigoe:isSpawned(), numChigoesToSpawn) then
                chigoe:setSpawn(hostMob:getXPos() + math.random(-2, 2), hostMob:getYPos() + math.random(-2, 2), hostMob:getZPos() + math.random(-2, 2), hostMob:getRotPos())
                chigoe:spawn()
                if target then
                    chigoe:updateEnmity(target)
                end

                chigoe:addListener('DISENGAGE', 'CHIGOE_DISENGAGE', function(mobArg)
                    DespawnMob(mobArg:getID())
                    mobArg:removeListener('CHIGOE_DISENGAGE')
                end)

                numChigoesToSpawn = numChigoesToSpawn - 1
                if numChigoesToSpawn == 0 then
                    return
                end
            end
        end
    end)
end

return g_mixins.families.chigoe_pet
