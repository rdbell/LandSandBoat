describe('Follower lifecycle helpers', function()
    it('spawns only missing adjacent followers', function()
        local spawned = {}
        stub('GetMobByID', function(id)
            if id == 104 then return nil end
            return {
                isSpawned = function() return id == 102 end,
            }
        end)
        stub('SpawnMob', function(id) spawned[#spawned + 1] = id end)

        xi.follow.spawnFollowers({ getID = function() return 100 end }, { [100] = 4 })

        assert(#spawned == 2 and spawned[1] == 101 and spawned[2] == 103)
    end)

    it('despawns only spawned, unengaged adjacent followers', function()
        local despawned = {}
        stub('GetMobByID', function(id)
            if id == 104 then return nil end
            return {
                isSpawned = function() return id ~= 103 end,
                isEngaged = function() return id == 102 end,
                getID = function() return id end,
            }
        end)
        stub('DespawnMob', function(id) despawned[#despawned + 1] = id end)

        xi.follow.despawnFollowers({
            getID = function() return 100 end,
            getMobMod = function(_, mod)
                assert(mod == xi.mobMod.LEADER)
                return 4
            end,
        })

        assert(#despawned == 1 and despawned[1] == 101)
    end)
end)
