describe('Spawn Handler', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
    end)

    describe('placeholder to NM', function()
        it('spawns NM when lottery succeeds', function()
            player:gotoZone(xi.zone.WEST_SARUTABARUTA)
            local ID = zones[xi.zone.WEST_SARUTABARUTA]
            local nm = player.entities:get(ID.mob.NUNYENUNC)
            local ph = player.entities:get(ID.mob.NUNYENUNC - 1)

            for _ = 1, 100 do
                if nm:isSpawned() then
                    break
                end

                player:claimAndKillMob(ph)
                xi.test.world:skipTime(305)
                xi.test.world:tick(xi.tick.SPAWN)
            end

            nm.assert:isSpawned()
        end)

        it('respawns placeholder when NM dies', function()
            player:gotoZone(xi.zone.WEST_SARUTABARUTA)
            local ID = zones[xi.zone.WEST_SARUTABARUTA]
            local nm = player.entities:get(ID.mob.NUNYENUNC)
            local ph = player.entities:get(ID.mob.NUNYENUNC - 1)

            for _ = 1, 100 do
                if nm:isSpawned() then
                    break
                end

                player:claimAndKillMob(ph)
                xi.test.world:skipTime(305)
                xi.test.world:tick(xi.tick.SPAWN)
            end

            player:claimAndKillMob(nm)
            xi.test.world:skipTime(305)
            xi.test.world:tick(xi.tick.SPAWN)
            ph.assert:isSpawned()
        end)
    end)
end)
