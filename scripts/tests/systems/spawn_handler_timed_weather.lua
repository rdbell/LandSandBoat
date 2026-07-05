describe('Spawn Handler', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
    end)

    describe('time-based spawns', function()
        it('spawns night mobs during night', function()
            player:gotoZone(xi.zone.ATTOHWA_CHASM)
            xi.test.world:setVanaTime(22, 0)

            local mob = player.entities:moveTo('Arch_Corse')
            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)

        it('blocks night mobs during day', function()
            player:gotoZone(xi.zone.ATTOHWA_CHASM)
            xi.test.world:setVanaTime(12, 0)

            local mob = player.entities:moveTo('Arch_Corse')
            mob:despawn()
            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert.no:isSpawned()
        end)

        it('spawns evening mobs during evening', function()
            xi.test.world:setVanaTime(18, 0)

            local mob = player.entities:moveTo('Ding_Bats')
            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)

        it('blocks evening mobs during day', function()
            xi.test.world:setVanaTime(12, 0)

            local mob = player.entities:moveTo('Ding_Bats')
            mob:despawn()
            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert.no:isSpawned()
        end)
    end)

    describe('weather-based spawns', function()
        it('spawns elementals when weather matches', function()
            player:gotoZone(xi.zone.CARPENTERS_LANDING)
            player:setWeather(xi.weather.THUNDER)

            local mob = player.entities:moveTo('Thunder_Elemental')
            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)

        it('blocks elementals when weather does not match', function()
            player:gotoZone(xi.zone.CARPENTERS_LANDING)
            player:setWeather(xi.weather.NONE)

            local mob = player.entities:moveTo('Thunder_Elemental')
            for _ = 1, 10 do
                xi.test.world:skipTime(5)
                xi.test.world:tickEntity(mob)
            end

            mob.assert.no:isSpawned()

            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert.no:isSpawned()
        end)

        it('spawns fog mobs during fog', function()
            player:setWeather(xi.weather.FOG)

            local mob = player.entities:moveTo('Bomb')
            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)

        it('blocks fog mobs without fog', function()
            player:setWeather(xi.weather.NONE)

            local mob = player.entities:moveTo('Bomb')
            for _ = 1, 10 do
                xi.test.world:skipTime(5)
                xi.test.world:tickEntity(mob)
            end

            mob.assert.no:isSpawned()

            mob:setRespawnTime(1)
            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert.no:isSpawned()
        end)
    end)

    describe('weather change despawns', function()
        it('despawns elementals when weather changes', function()
            player:gotoZone(xi.zone.CARPENTERS_LANDING)
            player:setWeather(xi.weather.THUNDER)

            local mob = player.entities:moveTo('Thunder_Elemental')
            mob:respawn()
            xi.test.world:tick()

            player:setWeather(xi.weather.NONE)
            for _ = 1, 10 do
                xi.test.world:skipTime(5)
                xi.test.world:tickEntity(mob)
            end

            mob.assert.no:isSpawned()
        end)

        it('despawns fog mobs when fog ends', function()
            player:setWeather(xi.weather.FOG)

            local mob = player.entities:moveTo('Bomb')
            mob:respawn()
            xi.test.world:tick()

            player:setWeather(xi.weather.NONE)
            for _ = 1, 10 do
                xi.test.world:skipTime(5)
                xi.test.world:tickEntity(mob)
            end

            mob.assert.no:isSpawned()
        end)
    end)

    describe('spawn wave window', function()
        it('only spawns mobs within the 15 second window', function()
            local mob = player.entities:moveTo('Forest_Funguar')
            mob:setRespawnTime(50)
            mob:despawn()

            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert.no:isSpawned()

            xi.test.world:tick(xi.tick.SPAWN)
            mob.assert:isSpawned()
        end)
    end)

    -- Xolotl in Attohwa Chasm: night spawn with 21-24 hour respawn timer
end)
