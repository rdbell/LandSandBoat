describe('Rise of the Zilart', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer()
    end)

    describe('ZM1 to ZM3', function()
        it('should complete ZM1 - The New Frontier', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_NEW_FRONTIER)
            player:setRank(6)
            player.assert:hasNationRank(6)

            -- After defeating the Shadow Lord and gaining rank 6, head to Norg for a cut-scene.
            player:gotoZone(xi.zone.NORG)
            player.events:expect({ eventId = 1 })
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.WELCOME_TNORG)
        end)

        it("should complete ZM2 - Welcome T'Norg", function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.WELCOME_TNORG)

            -- Go to L-8 and click on the "Oaken Door" to get a cutscene with Gilgamesh.
            player:gotoZone(xi.zone.NORG)
            player.entities:gotoAndTrigger('_700', { eventId = 2 }) -- Oaken Door
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.KAZHAMS_CHIEFTAINESS)
        end)

        it("should complete ZM3 - Kazham's Chieftainess", function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.KAZHAMS_CHIEFTAINESS)

            -- Talk to Jakoh Wahcondalo at (J-9) in Kazham to obtain the Key ItemSacrificial Chamber Key,
            -- which is required to enter the deeper areas of the Temple of Uggalepih.
            player:gotoZone(xi.zone.KAZHAM)
            player.entities:gotoAndTrigger('Jakoh_Wahcondalo', { eventId = 114 })
            player.assert:hasKI(xi.ki.SACRIFICIAL_CHAMBER_KEY)
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_TEMPLE_OF_UGGALEPIH)
        end)
    end)

    describe('ZM4 - The Temple of Uggalepih', function()
        it('should complete the Sacrificial Chamber battle', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_TEMPLE_OF_UGGALEPIH)
            player:addKeyItem(xi.ki.SACRIFICIAL_CHAMBER_KEY)

            -- After entering the Sacrificial Chamber, examine the heavy door to enter the Battlefield.
            player:gotoZone(xi.zone.SACRIFICIAL_CHAMBER)
            player.bcnm:enter('_4j0', xi.battlefield.id.TEMPLE_OF_UGGALEPIH)
            player.bcnm:killMobs()
            player.bcnm:expectWin()

            player.events:expect({ eventId = 7 })
            player.events:expect({ eventId = 8 })

            player.assert.no:hasKI(xi.ki.SACRIFICIAL_CHAMBER_KEY)
            player.assert:hasKI(xi.ki.DARK_FRAGMENT)
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.HEADSTONE_PILGRIMAGE)
        end)
    end)

end)
