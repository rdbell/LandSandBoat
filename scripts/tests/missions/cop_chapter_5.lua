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

    describe('5-1 The Enduring Tumult of War', function()
        it('should complete the mission successfully', function()
            local ID = zones[xi.zone.PSOXJA]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_ENDURING_TUMULT_OF_WAR)

            player:gotoZone(xi.zone.PORT_BASTOK)
            player.events:expect({ eventId = 306 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 849 })
            player.entities:gotoAndTrigger('Cid', { eventId = 863 })

            player:gotoZone(xi.zone.PSOXJA, { x = -300, y = 0, z = 0 })
            -- Note: gotoZone isn't well suited for CS that requires careful positioning on zone in yet.
            -- Force event
            player:startEvent(1)
            player.events:expect({ eventId = 1 })

            player.entities:gotoAndTrigger('_i98')
            local golem = player.entities:get(ID.mob.NUNYUNUWI)
            golem.assert:isSpawned()

            player:claimAndKillMob(golem)

            player.entities:gotoAndTrigger('_i99', { eventId = 2, finishOption = 1 })
            player.assert:inZone(xi.zone.PROMYVION_VAHZL)
            player.events:expect({ eventId = 50 })
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.DESIRES_OF_EMPTINESS)
                :hasKI(xi.ki.LIGHT_OF_VAHZL)
        end)
    end)

    describe('5-2 Desires of Emptiness', function()
        it('should complete the mission successfully', function()
            local ID = zones[xi.zone.PROMYVION_VAHZL]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.DESIRES_OF_EMPTINESS)

            player:gotoZone(xi.zone.PROMYVION_VAHZL)

            player.entities:gotoAndTrigger('_0mc')
            local propagator = player.entities:get(ID.mob.PROPAGATOR)
            propagator.assert:isSpawned()
            player:claimAndKillMob(propagator)
            player.entities:gotoAndTrigger('_0mc', { eventId = 51 })

            player.entities:gotoAndTrigger('_0md')
            local policitor = player.entities:get(ID.mob.SOLICITOR)
            policitor.assert:isSpawned()
            player:claimAndKillMob(policitor)
            player.entities:gotoAndTrigger('_0md', { eventId = 52 })

            player.entities:gotoAndTrigger('_0m0')
            local ponderer = player.entities:get(ID.mob.PONDERER)
            ponderer.assert:isSpawned()
            player:claimAndKillMob(ponderer)
            player.entities:gotoAndTrigger('_0m0', { eventId = 53 })

            player:gotoZone(xi.zone.SPIRE_OF_VAHZL)
            player.events:expect({ eventId = 20 })

            player.bcnm:enter('_0n0', xi.battlefield.id.DESIRES_OF_EMPTINESS)
            player.bcnm:killMobs()
            xi.test.world:skipTime(15)
            xi.test.world:tick()
            -- Player is sent to Beaucedine Glacier at end of event
            player.bcnm:expectWin({ finishOption = 2 })
            player.events:expect({ eventId = 206 })
            player.assert:inZone(xi.zone.BEAUCEDINE_GLACIER)

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 850 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THREE_PATHS)
        end)
    end)

    describe('5-3 Three Paths', function()
        it('should complete all three paths successfully', function()
            local upperID = zones[xi.zone.LOWER_DELKFUTTS_TOWER]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THREE_PATHS)

            -- Louverance's Path
            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('Despachiaire', { eventId = 118 })

            player:gotoZone(xi.zone.WINDURST_WOODS)
            player.entities:gotoAndTrigger('Perih_Vashai', { eventId = 686 })

            player:gotoZone(xi.zone.BIBIKI_BAY)
            player.entities:gotoAndTrigger('Warmachine', { eventId = 33 })

            player:gotoZone(xi.zone.WINDURST_WALLS)
            player.entities:gotoAndTrigger('Yoran-Oran', { eventId = 481 })

            player:gotoZone(xi.zone.OLDTON_MOVALPOLOS)
            player.events:expect({ eventId = 1 })

            player:gotoZone(xi.zone.MINE_SHAFT_2716)
            player.bcnm:enter('_0d0', xi.battlefield.id.CENTURY_OF_HARDSHIP)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 852 })

            player:gotoZone(xi.zone.OLDTON_MOVALPOLOS)
            player.entities:gotoAndTrigger('Tarnotik', { eventId = 34 })

            player:addItem(xi.item.GOLD_KEY)
            player:gotoZone(xi.zone.MINE_SHAFT_2716)
            player.actions:tradeNpc('_0d0', { xi.item.GOLD_KEY }, { eventId = 3 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 853 })

            -- Tenzen's Path
            player:gotoZone(xi.zone.LA_THEINE_PLATEAU)
            player.entities:gotoAndTrigger('qm3', { eventId = 203 })

            player:gotoZone(xi.zone.PSOXJA)
            player.entities:gotoAndTrigger('_09g', { eventId = 3 })

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('Monberaux', { eventId = 74 })

            player:gotoZone(xi.zone.RULUDE_GARDENS)
            player.entities:gotoAndTrigger('Pherimociel', { eventId = 58 })

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('Monberaux', { eventId = 6 })

            player:gotoZone(xi.zone.BATALLIA_DOWNS)
            player.entities:gotoAndTrigger('qm4', { eventId = 0 })
            player.entities:gotoAndTrigger('qm4', { eventId = 1 })
            player.assert:hasKI(xi.ki.DELKFUTT_RECOGNITION_DEVICE)

            player:gotoZone(xi.zone.LOWER_DELKFUTTS_TOWER)
            player.entities:gotoAndTrigger('_545')
            local idol = player.entities:get(upperID.mob.DISASTER_IDOL)
            idol.assert:isSpawned()
            player:claimAndKillMob(idol)
            xi.test.world:tick()
            player.entities:gotoAndTrigger('_545', { eventId = 25 })
            -- TODO: Are we supposed to lose the KI?
            -- player.assert.no:hasKI(xi.ki.DELKFUTT_RECOGNITION_DEVICE)

            player:gotoZone(xi.zone.PSOXJA)
            -- PsoXja CS conditions suck, force the event
            -- player:gotoZone(xi.zone.PSOXJA, { x = 220, y = -8, z = -282 })
            player:startEvent(4)
            player.events:expect({ eventId = 4 })

            player.entities:gotoAndTrigger('_09h', { eventId = 5 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 854 })

            -- Ulmia's Path
            player:gotoZone(xi.zone.SOUTHERN_SAN_DORIA)
            player.entities:gotoAndTrigger('Hinaree', { eventId = 22 })

            player:gotoZone(xi.zone.PORT_SAN_DORIA)
            player.events:expect({ eventId = 4 })

            player:gotoZone(xi.zone.NORTHERN_SAN_DORIA)
            player.entities:gotoAndTrigger('Chasalvige', { eventId = 762 })

            player:gotoZone(xi.zone.WINDURST_WATERS)
            player.entities:gotoAndTrigger('Kerutoto', { eventId = 876 })

            player:gotoZone(xi.zone.WINDURST_WALLS)
            player.entities:gotoAndTrigger('Yoran-Oran', { eventId = 473 })

            player:gotoZone(xi.zone.BONEYARD_GULLY)
            player.bcnm:enter('_081', xi.battlefield.id.HEAD_WIND)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            -- TODO: BCNM not working?
            --             player:gotoZone(xi.zone.BEARCLAW_PINNACLE)
            --             player.bcnm:enter('Wind_Pillar_1', xi.battlefield.id.FLAMES_FOR_THE_DEAD)
            --             player.bcnm:killMobs()
            --             player.bcnm:expectWin({ finishOption = 2 })
            --
            --             player:gotoZone(xi.zone.METALWORKS)
            --             player.entities:gotoAndTrigger('Cid', { eventId = 855 })
            --
            --             player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.FOR_WHOM_THE_VERSE_IS_SUNG)
        end)
    end)

end)
