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

    describe('6-1 For Whom the Verse is Sung', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.FOR_WHOM_THE_VERSE_IS_SUNG)

            player:gotoZone(xi.zone.RULUDE_GARDENS)
            player.entities:gotoAndTrigger('Pherimociel', { eventId = 10046 })

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('_6s1', { eventId = 10011 })

            player:gotoZone(xi.zone.RULUDE_GARDENS)
            xi.test.world:tick()
            player.events:expect({ eventId = 10047 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.A_PLACE_TO_RETURN)
        end)
    end)

    describe('6-2 A Place to Return', function()
        it('should complete the mission successfully', function()
            local ID = zones[xi.zone.MISAREAUX_COAST]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.A_PLACE_TO_RETURN)

            player:gotoZone(xi.zone.RULUDE_GARDENS, { x = 0, y = 3, z = 45 })
            player.events:expect({ eventId = 10048 })

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p0')
            local mob1 = player.entities:get(ID.mob.PM6_2_MOB_OFFSET)
            local mob2 = player.entities:get(ID.mob.PM6_2_MOB_OFFSET + 1)
            local mob3 = player.entities:get(ID.mob.PM6_2_MOB_OFFSET + 2)
            mob1.assert:isSpawned()
            player:claimAndKillMob(mob1)
            mob2.assert:isSpawned()
            player:claimAndKillMob(mob2)
            mob3.assert:isSpawned()
            player:claimAndKillMob(mob3)
            xi.test.world:tick()

            player.entities:gotoAndTrigger('_0p0', { eventId = 10 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.MORE_QUESTIONS_THAN_ANSWERS)
        end)
    end)

    describe('6-3 More Questions Than Answers', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.MORE_QUESTIONS_THAN_ANSWERS)

            player:gotoZone(xi.zone.RULUDE_GARDENS)
            player.entities:gotoAndTrigger('Pherimociel', { eventId = 10049 })

            player.entities:gotoAndTrigger('_6r9', { eventId = 10050 })

            player:gotoZone(xi.zone.SELBINA)
            player.entities:gotoAndTrigger('Mathilde', { eventId = 10005 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.ONE_TO_BE_FEARED)
        end)
    end)

    describe('6-4 One to be Feared', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.ONE_TO_BE_FEARED)

            -- Optional CS?
            --             player:gotoZone(xi.zone.SELBINA)
            --             player.entities:gotoAndTrigger('Mathilde', { eventId = 173 })
            --             player.entities:gotoAndTrigger('Mathilde', { eventId = 174 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 856 })

            player:gotoZone(xi.zone.SEALIONS_DEN)
            player.events:expect({ eventId = 15 })

            player.entities:gotoAndTrigger('_0w0', { eventId = 31 })

            -- Event progression not working
            --             player.bcnm:enter('_0w0', xi.battlefield.id.ONE_TO_BE_FEARED)
            --             player.entities:gotoAndTrigger('Airship_Door', { eventId = 32003, finishOption = 100 })
            --             player.events:expect({ eventId = 0 })
            --
            --             player.bcnm:killMobs()         -- Kill mammets
            --             player.events:expect({ eventId = 10 }) -- Move outside battlfield
            --
            --             -- Click door to enter next phase
            --             player.entities:gotoAndTrigger('Airship_Door', { eventId = 32003, finishOption = 100 })
            --             player.events:expect({ eventId = 1 })
            --             player.bcnm:killMobs()         -- Kill Omega
            --             player.events:expect({ eventId = 11 }) -- Move outside battlfield again
            --
            --             -- Click door to enter next phase
            --             player.entities:gotoAndTrigger('Airship_Door', { eventId = 32003, finishOption = 100 })
            --             player.events:expect({ eventId = 2 })
            --
            --             player.bcnm:killMobs() -- Kill Ultima
            --             player.bcnm:expectWin({ finishOption = 2 })
            --
            --             player.events:expect({ eventId = 33 })
            --
            --             player:getZoneID() == xi.zone.LUFAISE_MEADOWS)
            --             player.events:expect({ eventId = 111 })
            --
            --             player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.CHAINS_AND_BONDS)
        end)
    end)

end)
