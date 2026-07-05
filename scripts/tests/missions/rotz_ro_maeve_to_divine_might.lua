describe('Rise of the Zilart', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer()
    end)

    describe('ZM9 to ZM13', function()
        it("should complete ZM9 - Ro'Maeve", function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.ROMAEVE)
            -- Next head to Norg to talk to Gilgamesh.
            player:gotoZone(xi.zone.NORG)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('_700', { eventId = 3 }) -- Oaken Door
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_TEMPLE_OF_DESOLATION)
        end)

        it('should complete ZM10 - The Temple of Desolation', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_TEMPLE_OF_DESOLATION)

            -- Observe the gate at the other end of Hall of the Gods twice.
            player:gotoZone(xi.zone.HALL_OF_THE_GODS)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('_6z0', { eventId = 1 })
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_HALL_OF_THE_GODS)
        end)

        it('should complete ZM11 - The Hall of the Gods', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_HALL_OF_THE_GODS)

            -- .. go back to Norg and talk to Gilgamesh.
            player:gotoZone(xi.zone.NORG)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('_700', { eventId = 169 }) -- Oaken Door
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_MITHRA_AND_THE_CRYSTAL)
        end)

        it('should complete ZM12 - The Mithra and the Crystal', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_MITHRA_AND_THE_CRYSTAL)

            -- Go to Rabao and talk to Maryoh Comyujah, who's standing in front of the windmill at G-7.
            player:gotoZone(xi.zone.RABAO)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('Maryoh_Comyujah', { eventId = 81, finishOption = 1 })

            -- .. zone into Quicksand Caves.
            player:gotoZone(xi.zone.QUICKSAND_CAVES)
            -- Touch the ??? and select Yes to spawn the Ancient Vessel.
            player.entities:gotoAndTrigger('qm7', { eventId = 12, finishOption = 1 })
            player.assert.no:hasKI(xi.ki.SCRAP_OF_PAPYRUS)

            -- Kill the Ancient Vessel and inspect the ??? again to dig out the Scrap of Papyrus (key item).
            player:claimAndKillMob('Ancient_Vessel')
            player.entities:gotoAndTrigger('qm7', { eventId = 13, finishOption = 1 })
            player.assert:hasKI(xi.ki.SCRAP_OF_PAPYRUS)

            -- Return it to Maryoh Comyujah who will give you the Cerulean Crystal (key item).
            player:gotoZone(xi.zone.RABAO)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('Maryoh_Comyujah', { eventId = 83 })
            player.assert.no:hasKI(xi.ki.SCRAP_OF_PAPYRUS)
            player.assert:hasKI(xi.ki.CERULEAN_CRYSTAL)
        end)

        it('should complete ZM13 - The Gate of the Gods', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_MITHRA_AND_THE_CRYSTAL)
            player:addKeyItem(xi.ki.CERULEAN_CRYSTAL)

            -- Head back to the Hall of the Gods and touch the sealed gate and watch the cutscenes.
            player:gotoZone(xi.zone.HALL_OF_THE_GODS)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('_6z0', { eventId = 4 })
            -- There's two Shimmering Circles, so have to pick the lower one.
            player.entities:gotoAndTrigger(17805319, { eventId = 3 })
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_GATE_OF_THE_GODS)

            -- Note: You will also have to touch the Portal to Sky for the last CS.
            player:gotoZone(xi.zone.RUAUN_GARDENS)
            player.events:expect({ eventId = 51 })
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.ARK_ANGELS)
        end)
    end)

    describe('ZM14 - Ark Angels via Divine Might', function()
        it('should complete Divine Might battle', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.ARK_ANGELS)
            player:addQuest(xi.questLog.OUTLANDS, xi.quest.id.outlands.DIVINE_MIGHT)

            -- Take the main entrance to the Shrine of Ru'Avitau and run straight until you find an unmarked target.
            player:gotoZone(xi.zone.THE_SHRINE_OF_RUAVITAU)
            player.entities:gotoAndTrigger('blank_divine_might', { eventId = 53 })

            player:addItem(xi.item.ARK_PENTASPHERE)

            -- Go to La'Loff Amphitheater and use the Ark Pentasphere to enter a BC where you will confront and defeat all 5 Ark Angels.
            player:gotoZone(xi.zone.LALOFF_AMPHITHEATER)
            player.bcnm:enter('qm1_1', xi.battlefield.id.DIVINE_MIGHT, { xi.item.ARK_PENTASPHERE })
            player.bcnm:killMobs()
            player.bcnm:expectWin()

            player.assert:hasKI(xi.ki.SHARD_OF_APATHY)
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_SEALED_SHRINE)
        end)
    end)

end)
