describe('Spawn Handler', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
    end)

    describe('basic respawns', function()
        it('respawns a killed mob after its timer expires', function()
            local mob = player.entities:moveTo('Forest_Funguar')
            player:claimAndKillMob(mob)
            xi.test.world:skipTime(305)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)

        it('does not respawn a mob before its timer expires', function()
            local mob = player.entities:moveTo('Forest_Funguar')
            player:claimAndKillMob(mob)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert.no:isSpawned()
        end)

        it('respawns after deaggro with 60 second timer', function()
            local mob = player.entities:moveTo('River_Crab')
            mob:setPos(mob:getXPos() + 200, mob:getYPos(), mob:getZPos())
            mob:disengage()

            for _ = 1, 10 do
                xi.test.world:skipTime(5)
                xi.test.world:tickEntity(mob)
            end

            mob.assert.no:isSpawned()

            xi.test.world:skipTime(65)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)
    end)

    describe('spawn slots', function()
        it('spawns exactly one mob from a slot', function()
            player:gotoZone(xi.zone.GHELSBA_OUTPOST)
            local mobs = xi.test.world:getSpawnSlot(xi.zone.GHELSBA_OUTPOST, 1)
            local mob1 = mobs[1]
            local mob2 = mobs[2]

            mob1:despawn()
            mob2:despawn()
            mob1:setRespawnTime(1)
            mob2:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)

            assert(mob1:isSpawned() ~= mob2:isSpawned(), 'expected only one mob to be spawned from slot')
        end)

        it('respawns same mob after deaggro', function()
            player:gotoZone(xi.zone.GHELSBA_OUTPOST)
            local mobs  = xi.test.world:getSpawnSlot(xi.zone.GHELSBA_OUTPOST, 1)
            local mob1  = mobs[1]
            local mob2  = mobs[2]

            local mob   = mob1:isSpawned() and mob1 or mob2
            local other = mob == mob1 and mob2 or mob1

            for _ = 1, 5 do
                mob:setPos(mob:getXPos() + 200, mob:getYPos(), mob:getZPos())
                mob:disengage()

                for _ = 1, 10 do
                    xi.test.world:skipTime(5)
                    xi.test.world:tickEntity(mob)
                end

                mob.assert.no:isSpawned()

                xi.test.world:skipTime(65)
                xi.test.world:tick(xi.tick.SPAWN)
                mob.assert:isSpawned()
                other.assert.no:isSpawned()
            end
        end)
    end)

end)
