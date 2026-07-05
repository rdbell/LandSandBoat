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

    describe('3-1 The Call of the Wyrmking', function()
        it('should complete cutscenes in Bastok and advance mission', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_CALL_OF_THE_WYRMKING)

            player:gotoZone(xi.zone.SOUTH_GUSTABERG)
            player.events:expect({ eventId = 906 })

            player:gotoZone(xi.zone.PORT_BASTOK, { x = -100, y = 0, z = -10 })
            player.events:expect({ eventId = 305 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { eventId = 845 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.A_VESSEL_WITHOUT_A_CAPTAIN)
        end)
    end)

    describe('3-2 A Vessel Without a Captain', function()
        it('should complete story progression in Jeuno', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.A_VESSEL_WITHOUT_A_CAPTAIN)

            player:gotoZone(xi.zone.LOWER_JEUNO)
            player.entities:gotoAndTrigger('_6tc', { eventId = 86 })

            player:gotoZone(xi.zone.RULUDE_GARDENS)
            -- Unimplemented optional CSs?
            -- player.entities:gotoAndTrigger('Auchefort', { eventId = 6 })
            -- player.entities:gotoAndTrigger('Pherimociel', { eventId = 26 })

            player:gotoZone(xi.zone.RULUDE_GARDENS, { x = 0, y = 3, z = 45 })
            player.events:expect({ eventId = 65 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_ROAD_FORKS)
        end)
    end)

    describe('3-3 The Road Forks', function()
        it("should complete both San d'Oria and Windurst paths", function()
            local carpenterID = zones[xi.zone.CARPENTERS_LANDING]
            local chasmID = zones[xi.zone.ATTOHWA_CHASM]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_ROAD_FORKS)

            -- 1st Path
            player:gotoZone(xi.zone.NORTHERN_SAN_DORIA)
            player.events:expect({ event = 14 })
            player.entities:gotoAndTrigger('Arnau', { eventId = 51 })
            player.entities:gotoAndTrigger('Chasalvige', { eventId = 38 })

            player:gotoZone(xi.zone.CARPENTERS_LANDING)
            player.entities:gotoAndTrigger('Guilloud')
            xi.test.world:tick()
            local ivy = player.entities:get(carpenterID.mob.OVERGROWN_IVY)
            ivy.assert:isSpawned()

            player:claimAndKillMob(ivy)
            xi.test.world:tick()

            player.entities:gotoAndTrigger('Guilloud', { event = 0 })

            player:gotoZone(xi.zone.SOUTHERN_SAN_DORIA)
            player.entities:gotoAndTrigger('Hinaree', { event = 23 })
            player.entities:gotoAndTrigger('Hinaree', { event = 24 })

            -- 2nd Path
            player:gotoZone(xi.zone.WINDURST_WATERS)
            player.events:expect({ event = 871 })
            player.entities:gotoAndTrigger('Ohbiru-Dohbiru', { event = 872 })

            player:gotoZone(xi.zone.WINDURST_WALLS)
            player.entities:gotoAndTrigger('Yoran-Oran', { event = 469 })

            player:gotoZone(xi.zone.WINDURST_WATERS)
            player.entities:gotoAndTrigger('Kyume-Romeh', { event = 873 })
            player.entities:gotoAndTrigger('Honoi-Gomoi', { event = 874 })
            player.assert:hasKI(xi.ki.CRACKED_MIMEO_MIRROR)

            player:gotoZone(xi.zone.WINDURST_WALLS)
            player.entities:gotoAndTrigger('Yoran-Oran', { eventId = 470 })
            player.assert.no:hasKI(xi.ki.CRACKED_MIMEO_MIRROR)

            player:gotoZone(xi.zone.ATTOHWA_CHASM)
            player.entities:gotoAndTrigger('Loose_Sand')
            xi.test.world:tick()
            local mob2 = player.entities:get(chasmID.mob.LIOUMERE)
            mob2.assert:isSpawned()

            player:claimAndKillMob(mob2)
            xi.test.world:tick()

            player.entities:gotoAndTrigger('Loose_Sand')
            player.assert:hasKI(xi.ki.MIMEO_JEWEL)

            player.entities:gotoAndTrigger('Cradle_of_Rebirth', { event = 2 })
            player.assert.no:hasKI(xi.ki.MIMEO_JEWEL)
            player.assert:hasKI(xi.ki.MIMEO_FEATHER)
            player.assert:hasKI(xi.ki.SECOND_MIMEO_FEATHER)
            player.assert:hasKI(xi.ki.THIRD_MIMEO_FEATHER)

            player:gotoZone(xi.zone.WINDURST_WALLS)
            player.entities:gotoAndTrigger('Yoran-Oran', { event = 471 })
            player.assert.no:hasKI(xi.ki.MIMEO_FEATHER)
            player.assert.no:hasKI(xi.ki.SECOND_MIMEO_FEATHER)
            player.assert.no:hasKI(xi.ki.THIRD_MIMEO_FEATHER)

            player:gotoZone(xi.zone.PORT_WINDURST)
            player.entities:gotoAndTrigger('Yujuju', { event = 592 })

            player:gotoZone(xi.zone.WINDURST_WATERS)
            player.entities:gotoAndTrigger('Tosuka-Porika', { event = 875 })

            player:gotoZone(xi.zone.WINDURST_WALLS)
            player.entities:gotoAndTrigger('Yoran-Oran', { event = 472 })

            player:gotoZone(xi.zone.METALWORKS)
            player.entities:gotoAndTrigger('Cid', { event = 847 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.TENDING_AGED_WOUNDS)
        end)
    end)

    describe('3-4 Tending Aged Wounds', function()
        it('should complete story progression in Lower Jeuno', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.TENDING_AGED_WOUNDS)

            player:gotoZone(xi.zone.LOWER_JEUNO)
            player.events:expect({ event = 70 })

            player.entities:gotoAndTrigger('_6tc', { event = 22 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.DARKNESS_NAMED)
        end)
    end)

    describe('3-5 Darkness Named', function()
        it('should complete Gray Chip quest and defeat Shrouded Maw BCNM', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.DARKNESS_NAMED)

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('Monberaux', { event = 82 })

            player:gotoZone(xi.zone.LOWER_JEUNO)
            player.entities:gotoAndTrigger('Ghebi_Damomohe', { event = 54 })
            player.entities:gotoAndTrigger('Ghebi_Damomohe', { event = 53 })

            player:addItem(xi.item.GRAY_CHIP)
            player.actions:tradeNpc('Ghebi_Damomohe', { xi.item.GRAY_CHIP }, { eventId = 52 })
            player.assert:hasKI(xi.ki.PSOXJA_PASS)

            player:gotoZone(xi.zone.THE_SHROUDED_MAW)
            player.events:expect({ event = 2 })

            player.bcnm:enter('MC_Entrance', xi.battlefield.id.DARKNESS_NAMED)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('Monberaux', { event = 75 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.SHELTERING_DOUBT)
        end)
    end)

end)
