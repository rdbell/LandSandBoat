describe('Spawn Handler', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
    end)

    describe('timed night spawns', function()
        it('does not respawn killed Xolotl on next night', function()
            player:gotoZone(xi.zone.ATTOHWA_CHASM)
            xi.test.world:setVanaTime(22, 0)

            local ID = zones[xi.zone.ATTOHWA_CHASM]
            local xolotl = player.entities:get(ID.mob.XOLOTL)
            xolotl:respawn()
            xi.test.world:tick()

            player:claimAndKillMob(xolotl)

            -- Skip to next night (about 24 vana hours)
            xi.test.world:setVanaTime(22, 0)
            xi.test.world:tick(xi.tick.SPAWN)

            xolotl.assert.no:isSpawned()
        end)

        it('respawns Xolotl after natural despawn at dawn', function()
            player:gotoZone(xi.zone.ATTOHWA_CHASM)
            xi.test.world:setVanaTime(22, 0)

            local ID = zones[xi.zone.ATTOHWA_CHASM]
            local xolotl = player.entities:get(ID.mob.XOLOTL)
            xolotl:respawn()
            xi.test.world:tick()

            xolotl.assert:isSpawned()

            -- Move to dawn (4:00) - should trigger natural despawn
            xi.test.world:setVanaTime(4, 0)
            xi.test.world:tick(xi.tick.TIME)
            for _ = 1, 10 do
                xi.test.world:skipTime(5)
                xi.test.world:tickEntity(xolotl)
            end

            xolotl.assert.no:isSpawned()

            -- Next night it should spawn
            xi.test.world:setVanaTime(22, 0)
            xi.test.world:tick(xi.tick.SPAWN)

            xolotl.assert:isSpawned()
        end)
    end)

    describe('scripted spawns', function()
        it('respawns Simurgh after kill within respawn window', function()
            player:gotoZone(xi.zone.ROLANBERRY_FIELDS)
            local ID = zones[xi.zone.ROLANBERRY_FIELDS]
            local simurgh = player.entities:get(ID.mob.SIMURGH)

            -- Initial spawn
            xi.test.world:skipTime(7200)
            xi.test.world:tick(xi.tick.SPAWN)
            simurgh.assert:isSpawned()

            player:claimAndKillMob(simurgh)

            -- Skip 2 hours again for respawn
            xi.test.world:skipTime(7200)
            xi.test.world:tick(xi.tick.SPAWN)

            simurgh.assert:isSpawned()
        end)

        it('King Arthro spawns when all Knight Crabs killed and blocks their respawn', function()
            player:gotoZone(xi.zone.JUGNER_FOREST)
            local ID = zones[xi.zone.JUGNER_FOREST]
            local kingArthro = player.entities:get(ID.mob.KING_ARTHRO)

            -- Gather all 10 Knight Crabs and wait for initial spawn
            local knightCrabs = {}
            for offset = 1, 10 do
                knightCrabs[offset] = player.entities:get(ID.mob.KING_ARTHRO - offset)
            end

            xi.test.world:skipTime(12000)
            xi.test.world:tick(xi.tick.SPAWN)

            for _, crab in ipairs(knightCrabs) do
                crab.assert:isSpawned()
            end

            -- Kill all 10 Knight Crabs
            for _, crab in ipairs(knightCrabs) do
                player:claimAndKillMob(crab)
            end

            -- King Arthro should spawn
            xi.test.world:tick()
            kingArthro.assert:isSpawned()

            -- Let KA roam for some time - Knight Crabs should NOT respawn
            xi.test.world:skipTime(3600)
            xi.test.world:tick(xi.tick.SPAWN)
            for _, crab in ipairs(knightCrabs) do
                crab.assert.no:isSpawned()
            end

            -- Kill King Arthro
            player:claimAndKillMob(kingArthro)

            -- Skip past max respawn window (24h 10m)
            xi.test.world:skipTime(87000)
            xi.test.world:tick(xi.tick.SPAWN)

            -- Knight Crabs should now be respawned
            for _, crab in ipairs(knightCrabs) do
                crab.assert:isSpawned()
            end
        end)
    end)

    -- Nunyenunc NM and Carrion Crow PH in West Sarutabaruta
end)
