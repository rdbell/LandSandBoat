describe('Chains of Promathia', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer()
        player:setLevel(99)

        -- Mission conflicts
        player:addMission(xi.mission.log_id.ASA, xi.mission.id.asa.BURGEONING_DREAD)
        player:addMission(xi.mission.log_id.ACP, xi.mission.id.acp.A_CRYSTALLINE_PROPHECY_FIN)
        player:addMission(xi.mission.log_id.ROV, xi.mission.id.rov.A_RHAPSODY_FOR_THE_AGES)
        player:addMission(xi.mission.log_id.SOA, xi.mission.id.soa.ABOMINATION)
    end)

    describe('8-1 Garden of Antiquity', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.GARDEN_OF_ANTIQUITY)

            player:gotoZone(xi.zone.ALTAIEU)
            player.entities:gotoAndTrigger('_0x0', { eventId = 164 })

            local southMob1 = player.entities:get(16912829)
            local southMob2 = player.entities:get(16912829 + 1)
            local southMob3 = player.entities:get(16912829 + 2)
            player.entities:gotoAndTrigger('_0x1')
            southMob1.assert:isSpawned()
            southMob2.assert:isSpawned()
            southMob3.assert:isSpawned()
            player:claimAndKillMob(southMob1)
            player:claimAndKillMob(southMob2)
            player:claimAndKillMob(southMob3)
            player.entities:gotoAndTrigger('_0x1', { eventId = 161 })

            local westMob1 = player.entities:get(16912832)
            local westMob2 = player.entities:get(16912832 + 1)
            local westMob3 = player.entities:get(16912832 + 2)
            player.entities:gotoAndTrigger('_0x2')
            westMob1.assert:isSpawned()
            westMob2.assert:isSpawned()
            westMob3.assert:isSpawned()
            player:claimAndKillMob(westMob1)
            player:claimAndKillMob(westMob2)
            player:claimAndKillMob(westMob3)
            player.entities:gotoAndTrigger('_0x2', { eventId = 162 })

            local eastMob1 = player.entities:get(16912835)
            local eastMob2 = player.entities:get(16912835 + 1)
            local eastMob3 = player.entities:get(16912835 + 2)
            player.entities:gotoAndTrigger('_0x3')
            eastMob1.assert:isSpawned()
            eastMob2.assert:isSpawned()
            eastMob3.assert:isSpawned()
            player:claimAndKillMob(eastMob1)
            player:claimAndKillMob(eastMob2)
            player:claimAndKillMob(eastMob3)
            player.entities:gotoAndTrigger('_0x3', { eventId = 163 })

            player.entities:gotoAndTrigger('_0x0', { eventId = 100 })

            player:gotoZone(xi.zone.GRAND_PALACE_OF_HUXZOI)
            xi.test.world:tick()
            player.entities:gotoAndTrigger('_iya', { eventId = 1 })
            player.entities:gotoAndTrigger('_iyb', { eventId = 2 })
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.A_FATE_DECIDED)
        end)
    end)

    describe('8-2 A Fate Decided', function()
        it('should complete the mission successfully', function()
            local ID = zones[xi.zone.GRAND_PALACE_OF_HUXZOI]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.A_FATE_DECIDED)

            player:gotoZone(xi.zone.GRAND_PALACE_OF_HUXZOI)
            player.entities:gotoAndTrigger('_iyq')
            local mob = player.entities:get(ID.mob.IXGHRAH)
            mob.assert:isSpawned()
            player:claimAndKillMob(mob)
            player.entities:gotoAndTrigger('_iyq', { eventId = 3 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.WHEN_ANGELS_FALL)
        end)
    end)

    describe('8-3 When Angels Fall', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.WHEN_ANGELS_FALL)
            player:addKeyItem(xi.ki.BRAND_OF_DAWN)
            player:addKeyItem(xi.ki.BRAND_OF_TWILIGHT)

            player:gotoZone(xi.zone.THE_GARDEN_OF_RUHMET)
            xi.test.world:tick()
            player.events:expect({ eventId = 201 })
            player.assert:hasKI(xi.ki.MYSTERIOUS_AMULET_PRISHE)

            -- Hume Ebon Panel
            player.entities:gotoAndTrigger('_iz2', { eventId = 202 })

            -- Eden was missing this event
            player.entities:gotoAndTrigger('_iz2', { eventId = 120, finishOption = 1 })
            player.assert:hasKI(xi.ki.LIGHT_OF_VAHZL)

            player.entities:gotoAndTrigger('_0z0', { eventId = 203 })

            player.bcnm:enter('_0z0', xi.battlefield.id.WHEN_ANGELS_FALL)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            player.entities:gotoAndTrigger('_0zt', { eventId = 204 })

            -- Begin: Eden was missing this section
            player:gotoZone(xi.zone.ALTAIEU)
            player.events:expect({ eventId = 165 })
            player.assert.no:hasKI(xi.ki.MYSTERIOUS_AMULET_PRISHE)
            -- End: Eden was missing this section

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.DAWN)
        end)
    end)
end)
