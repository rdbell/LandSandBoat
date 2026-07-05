describe('Rise of the Zilart', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer()
    end)

    describe('ZM15 to ZM17 - Final Missions', function()
        it('should complete ZM15 - The Sealed Shrine', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_SEALED_SHRINE)

            -- Now go to Norg and talk to Gilgamesh.
            player:gotoZone(xi.zone.NORG)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('_700', { eventId = 172 }) -- Oaken Door

            -- Then go to Lower Jeuno and talk to Aldo in Tenshodo HQ J-8 for a cutscene.
            player:gotoZone(xi.zone.LOWER_JEUNO)
            player.events:expectNotInEvent()
            player.entities:gotoAndTrigger('Aldo', { eventId = 111 })

            -- Enter Shrine of Ru'Avitau again from the (H-8) entrance for a cutscene with Lion.
            player:gotoZone(xi.zone.THE_SHRINE_OF_RUAVITAU, { x = -40, y = -2, z = -230 })
            player.events:expect({ eventId = 51 })
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_CELESTIAL_NEXUS)
        end)

        it('should complete ZM16 - The Celestial Nexus', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_CELESTIAL_NEXUS)

            player:gotoZone(xi.zone.THE_CELESTIAL_NEXUS)
            player.bcnm:enter('_513', xi.battlefield.id.CELESTIAL_NEXUS)

            -- Phase 1
            local eald = player.entities:get(zones[xi.zone.THE_CELESTIAL_NEXUS].mob.EALDNARCHE)
            local exoplates = player.entities:get(zones[xi.zone.THE_CELESTIAL_NEXUS].mob.EALDNARCHE + 1)
            exoplates:setUnkillable(false)
            eald:setUnkillable(false)

            player:claimAndKillMob(exoplates)
            player:claimAndKillMob(eald)
            player.events:expect({ eventId = 32004 })

            -- Phase 2
            local eald2 = player.entities:get(zones[xi.zone.THE_CELESTIAL_NEXUS].mob.EALDNARCHE + 2)
            player:claimAndKillMob(eald2)
            player.bcnm:expectWin()

            -- After the final cutscene, you appear in Hall of the Gods.
            player.assert:inZone(xi.zone.HALL_OF_THE_GODS)
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.AWAKENING)
        end)

        it('should complete ZM17 - Awakening', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.AWAKENING)
            local statusBefore = player:getMissionStatus(xi.mission.log_id.ZILART)
            player:setMissionStatus(xi.mission.log_id.ZILART, 0) -- Reset mission status to 0
            local statusAfter = player:getMissionStatus(xi.mission.log_id.ZILART)
            print(string.format('[TEST DEBUG] Mission status - before reset: %d, after reset: %d',
                                statusBefore, statusAfter))

            -- Zone into Norg for a cutscene with Gilgamesh.
            player:gotoZone(xi.zone.NORG)
            player.events:expect({ eventId = 176 })

            -- Enter the Neptune's Spire in Lower Jeuno for a cutscene with Aldo.
            player:gotoZone(xi.zone.LOWER_JEUNO)
            player.entities:gotoAndTrigger('_6tc', { eventId = 20 }) -- Door to Neptune's Spire

            -- Start of Shadows of the Departed
            -- After the conquest tally walk back into the Ducal palace for a cutscene.
            player:completeQuest(xi.questLog.JEUNO, xi.quest.id.jeuno.STORMS_OF_FATE)
            -- TODO: Eden had y as 0, the trigger area may need to be adjusted.
            player:gotoZone(xi.zone.RULUDE_GARDENS, { x = 0, y = 3, z = 45 })
            player.events:expect({ eventId = 161 })
        end)
    end)
end)
