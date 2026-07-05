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

    describe('1-0 Ancient Flames Beckon', function()
        it("should progress through Lower Delkfutt's Tower cutscenes and advance to The Rites of Life", function()
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.ANCIENT_FLAMES_BECKON)

            -- # COP 0
            -- zone into Lower Delkfutts Tower for a series of CS's
            player:gotoZone(xi.zone.QUFIM_ISLAND)
            player:gotoZone(xi.zone.LOWER_DELKFUTTS_TOWER)

            -- TODO: check Tales' Beginning with option 0, talking to the Tales' Beginning, which starts CS 53.
            -- In CS 53, Send option 4 to accept CS, which will start CS 22 with out any options to select
            -- Also verify charvar for TalesBeginning
            player.events:expect({ eventId = 22, finishOption = 1 })
            player.events:expect({ eventId = 36 })
            player.events:expect({ eventId = 37 })
            player.events:expect({ eventId = 38 })
            player.events:expect({ eventId = 39 })
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_RITES_OF_LIFE)
        end)
    end)

    describe('1-1 The Rites of Life', function()
        it('should complete quest with Monberaux and advance to Below the Arks', function()
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_RITES_OF_LIFE)
            -- This var is set automatically when completing the previous mission
            player:setCharVar(
                string.format('Mission[%d][%d]Status', xi.mission.log_id.COP, xi.mission.id.cop.THE_RITES_OF_LIFE), 1)

            -- # COP 1
            -- Zone into Upper Jeuno for a CS
            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.events:expect({ eventId = 2 })

            -- trigger Monberaux for a series of CS's complete quest and get KI
            player.entities:gotoAndTrigger('Monberaux', { eventId = 10 })
            player.events:expect({ eventId = 206 })
            player.events:expect({ eventId = 207 })
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.BELOW_THE_ARKS)
            player.assert:hasKI(xi.ki.MYSTERIOUS_AMULET)
        end)
    end)

    describe('1-2 Below the Arks - Holla', function()
        it('should complete Promyvion Holla and Spire battles to advance to The Mothercrystals', function()
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.BELOW_THE_ARKS)

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('Monberaux', { eventId = 9 })

            -- Ru'Lude Gardens: Trigger Pherimociel to goto next Prog
            player:gotoZone(xi.zone.RULUDE_GARDENS)
            player.entities:gotoAndTrigger('Pherimociel', { eventId = 24 })

            -- Ru'Lude Gardens: Optional dialog
            player.entities:gotoAndTrigger('Pherimociel', { eventId = 25 })
            player.entities:gotoAndTrigger('High_Wind', { eventId = 33 })
            player.entities:gotoAndTrigger('Rainhard', { eventId = 34 })

            -- entering hall of transference -> Promy Holla
            player:gotoZone(xi.zone.HALL_OF_TRANSFERENCE)
            -- TODO: Not seeing 108 on the capture
            player.events:expect({ eventId = 108 })

            player.entities:gotoAndTrigger('_0e3', { eventId = 160 })
            player.assert:inZone(xi.zone.PROMYVION_HOLLA)

            -- 1st time entering gets a CS
            player.events:expect({ eventId = 50 })

            -- Spire of Holla, trigger and enter BCNM, winning grants next mission
            player:gotoZone(xi.zone.SPIRE_OF_HOLLA)
            player.bcnm:enter('_0h0', xi.battlefield.id.ANCIENT_FLAMES_BECKON_SPIRE_OF_HOLLA)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })
            player.assert:hasKI(xi.ki.LIGHT_OF_HOLLA)
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_MOTHERCRYSTALS)
        end)
    end)

    describe('1-3 The Mothercrystals', function()
        it('should complete all three Promyvion battles and unlock teleports', function()
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_MOTHERCRYSTALS)
            player:addKeyItem(xi.ki.LIGHT_OF_HOLLA)

            -- entering next promy
            player:gotoZone(xi.zone.KONSCHTAT_HIGHLANDS)
            -- Touching the telepoint will warp us to Hall of Transference and then Promyvion-Dem
            player.entities:gotoAndTrigger('Shattered_Telepoint')
            player.events:expect({ eventId = 912 }) -- Hall of Transference
            player.events:expect({ eventId = 51 })  -- Promyvion-Dem
            player.assert:inZone(xi.zone.PROMYVION_DEM)

            -- Fight at BCNM
            player:gotoZone(xi.zone.SPIRE_OF_DEM)
            player.bcnm:enter('_0j0', xi.battlefield.id.ANCIENT_FLAMES_BECKON_SPIRE_OF_DEM)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })
            player.assert:hasKI(xi.ki.LIGHT_OF_DEM)

            -- going to next promy, cs inside hall of transference
            player:gotoZone(xi.zone.TAHRONGI_CANYON)
            player.entities:gotoAndTrigger('Shattered_Telepoint', { eventId = 913, finishOption = 0 })

            -- event upon entering hall
            player.assert:inZone(xi.zone.HALL_OF_TRANSFERENCE)
            player.events:expect({ eventId = 155 })

            -- event upon entering promy after Hall warp
            player.assert:inZone(xi.zone.PROMYVION_MEA)
            player.events:expect({ eventId = 52 })

            -- enter and beat BCNM
            player:gotoZone(xi.zone.SPIRE_OF_MEA)
            player.bcnm:enter('_0l0', xi.battlefield.id.ANCIENT_FLAMES_BECKON_SPIRE_OF_MEA)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            -- Should have Light of Mea and be teleported to Lufaise Meadows
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.AN_INVITATION_WEST)
                :hasKI(xi.ki.LIGHT_OF_MEA)
                :inZone(xi.zone.LUFAISE_MEADOWS)

            -- zone in cs
            player.events:expect({ eventId = 110 })
            player.entities:gotoAndTrigger('Swirling_Vortex', { eventId = 100 })

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('Swirling_Vortex', { eventId = 554 })

            player:gotoZone(xi.zone.QUFIM_ISLAND)
            player.entities:gotoAndTrigger('Swirling_Vortex', { eventId = 300 })

            player:gotoZone(xi.zone.VALKURM_DUNES)
            player.entities:gotoAndTrigger('Swirling_Vortex', { eventId = 12 })
        end)
    end)

end)
