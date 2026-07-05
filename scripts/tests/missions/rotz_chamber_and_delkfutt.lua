describe('Rise of the Zilart', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer()
    end)

    describe('ZM6 and ZM7 - Chamber of Oracles', function()
        it('should complete Through the Quicksand Caves and The Chamber of Oracles', function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THROUGH_THE_QUICKSAND_CAVES)
            local pedestalsFragmentsTable =
            {
                { xi.ki.FIRE_FRAGMENT,      'Pedestal_of_Fire' },
                { xi.ki.EARTH_FRAGMENT,     'Pedestal_of_Earth' },
                { xi.ki.WATER_FRAGMENT,     'Pedestal_of_Water' },
                { xi.ki.WIND_FRAGMENT,      'Pedestal_of_Wind' },
                { xi.ki.ICE_FRAGMENT,       'Pedestal_of_Ice' },
                { xi.ki.LIGHTNING_FRAGMENT, 'Pedestal_of_Lightning' },
                { xi.ki.LIGHT_FRAGMENT,     'Pedestal_of_Light' },
                { xi.ki.DARK_FRAGMENT,      'Pedestal_of_Darkness' },
            }
            for _, info in ipairs(pedestalsFragmentsTable) do
                player:addKeyItem(info[1])
            end

            -- Enter the Chamber of Oracles.
            player:gotoZone(xi.zone.CHAMBER_OF_ORACLES)
            player.bcnm:enter('SC_Entrance',
                xi.battlefield.id.THROUGH_THE_QUICKSAND_CAVES)
            player.bcnm:killMobs()
            player.bcnm:expectWin()

            -- Upon defeating the 3 NM Anticans you will be appear in another area of the Chamber of Oracles;
            -- this is the start of "ZM7 - The Chamber of Oracles".
            -- Place the fragments in the pedestals for cutscene.
            player.events:expectNotInEvent()
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THE_CHAMBER_OF_ORACLES)

            -- Place a fragment on each pedestal
            for idx, info in ipairs(pedestalsFragmentsTable) do
                player.assert:hasKI(info[1])
                player.entities:gotoAndTrigger(info[2])
                player.assert.no:hasKI(info[1])

                if idx ~= #pedestalsFragmentsTable then
                    -- Is not the last pedestal, so just a message is given
                    player.events:expectNotInEvent()
                else
                    -- Clicking the last pedestal starts cutscene
                    player.events:expect({ eventId = 1 })
                end
            end

            player.assert:hasKI(xi.ki.PRISMATIC_FRAGMENT)
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.RETURN_TO_DELKFUTTS_TOWER)
        end)
    end)

    describe("ZM8 - Return to Delkfutt's Tower", function()
        it("should defeat Archduke Kam'lanaut", function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.RETURN_TO_DELKFUTTS_TOWER)

            -- Go through the portal to Stellar Fulcrum. You will receive a cutscene.
            player:gotoZone(xi.zone.STELLAR_FULCRUM)
            player.events:expect({ eventId = 0 })

            -- Fight and defeat Archduke Kam'lanaut.
            player.bcnm:enter('_4z0', xi.battlefield.id.RETURN_TO_DELKFUTTS_TOWER)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 1 })

            -- When the battle has concluded, be prepared for a long cutscene (approx. 6 minutes).
            player.events:expect({ eventId = 17 })
        end)
    end)

end)
