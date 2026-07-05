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

    describe('2-1 An Invitation West', function()
        it('should lose amulet in Lufaise and advance to The Lost City', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.AN_INVITATION_WEST)
            player:addKeyItem(xi.ki.MYSTERIOUS_AMULET)

            -- zone in and lose amulet
            player:gotoZone(xi.zone.LUFAISE_MEADOWS)
            player.events:expect({ eventId = 110 })
            player.assert.no:hasKI(xi.ki.MYSTERIOUS_AMULET)

            -- zone in to gain next mission
            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.events:expect({ eventId = 101 })

            -- check if mission completes
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_LOST_CITY)
        end)
    end)

    describe('2-2 The Lost City', function()
        it('should complete NPC interactions in Tavnazian Safehold', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_LOST_CITY)

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('Despachiaire')
            player.events:expect({ eventId = 102 })

            player.entities:gotoAndTrigger('Liphatte')
            player.events:expect({ eventId = 301 })

            player.entities:gotoAndTrigger('Justinius')
            player.events:expect({ eventId = 360 })

            player.entities:gotoAndTrigger('_0q1')
            player.events:expect({ eventId = 103 })

            -- check if mission completes
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.DISTANT_BELIEFS)
        end)
    end)

    describe('2-3 Distant Beliefs', function()
        it('should defeat Minotour in Phomiuna Aqueducts', function()
            -- Sewer door needs this mission explicitly completed to trigger event
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_LOST_CITY)
            player:completeMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_LOST_CITY)
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.DISTANT_BELIEFS)

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('Justinius', { eventId = 123 })

            -- TODO: This trigger is not working. Supposed to warp to aqueducts
            -- player.entities:gotoAndTrigger('_0q1', { eventId = 502, finishOption = 1 })
            player:gotoZone(xi.zone.PHOMIUNA_AQUEDUCTS)

            xi.test.world:skipTime(900)
            xi.test.world:tick()
            player:claimAndKillMob('Minotaur')

            -- TODO: This is the right Wooden_Ladder to trigger but it's not working D:
            -- player.entities:gotoAndTrigger(16888103, { eventId = 35 })
            -- player.entities:gotoAndTrigger('_0r5', { eventId = 36 })

            -- force events
            player:startEvent(35)
            player.events:expect({ eventId = 35 })
            player:startEvent(36)
            player.events:expect({ eventId = 36 })

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('Justinius', { eventId = 113 })

            -- check if mission completes
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.AN_ETERNAL_MELODY)
        end)
    end)

    describe('2-4 An Eternal Melody', function()
        it('should complete NPC interactions and obtain Mysterious Amulet', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.AN_ETERNAL_MELODY)

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)

            -- TODO: Unimplemented optional CS?
            -- player.entities:gotoAndTrigger('Calengeard', { eventId = 395 })
            -- player.entities:gotoAndTrigger('Reaugettie', { eventId = 292 })

            player.entities:gotoAndTrigger('Justinius', { eventId = 125 })
            player.entities:gotoAndTrigger('_0qa', { eventId = 104 })
            player.assert:hasKI(xi.ki.MYSTERIOUS_AMULET)

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p0', { eventId = 5 })

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD, { x = -5, y = -24, z = 18 })
            xi.test.world:skipTime(1)
            xi.test.world:tick()
            player.events:expect({ eventId = 105 })

            -- check if mission completes
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.ANCIENT_VOWS)
        end)
    end)

    describe('2-5 Ancient Vows', function()
        it('should complete Monarch Linn BCNM and advance missions', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.ANCIENT_VOWS)

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p2')
            player.events:expect({ eventId = 6 })

            player:gotoZone(xi.zone.RIVERNE_SITE_A01)
            player.events:expect({ eventId = 100 })

            player:gotoZone(xi.zone.MONARCH_LINN)
            player.bcnm:enter('SD_Entrance', xi.battlefield.id.ANCIENT_VOWS)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            player.assert:inZone(xi.zone.SOUTH_GUSTABERG)
            player.events:expect({ eventId = 906 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_CALL_OF_THE_WYRMKING)
        end)
    end)

end)
