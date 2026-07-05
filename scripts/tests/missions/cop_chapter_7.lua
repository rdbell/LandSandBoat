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

    describe('7-1 Chains and Bonds', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.CHAINS_AND_BONDS)

            player:gotoZone(xi.zone.LUFAISE_MEADOWS)
            xi.test.world:tick()
            player.events:expect({ eventId = 111 })

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            xi.test.world:tick()
            xi.test.world:skipTime(2)
            player.events:expect({ eventId = 114 })
            player.entities:gotoAndTrigger('_0q1', { eventId = 116 })

            player:gotoZone(xi.zone.SEALIONS_DEN)
            xi.test.world:tick()
            player.events:expect({ eventId = 14 })

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('_0qa', { eventId = 115 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.FLAMES_IN_THE_DARKNESS)
        end)
    end)

    describe('7-2 Flames in the Darkness', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.FLAMES_IN_THE_DARKNESS)

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p2', { eventId = 12 })

            player:gotoZone(xi.zone.SEALIONS_DEN)
            player.entities:gotoAndTrigger('Sueleen', { eventId = 16 })

            player:gotoZone(xi.zone.RULUDE_GARDENS, { x = 0, y = 3, z = 45 })
            player.events:expect({ eventId = 10051 })

            player:gotoZone(xi.zone.UPPER_JEUNO)
            player.entities:gotoAndTrigger('_6s1', { eventId = 10012 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.FIRE_IN_THE_EYES_OF_MEN)
        end)
    end)

    describe('7-3 Fire in the Eyes of Men', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.FIRE_IN_THE_EYES_OF_MEN)

            player:gotoZone(xi.zone.MINE_SHAFT_2716)
            player.entities:gotoAndTrigger('_0d0', { eventId = 4 })

            player:gotoZone(xi.zone.METALWORKS)

            -- First CS that sets the timer for the next event
            player.entities:gotoAndTrigger('Cid', { eventId = 857 })

            -- Cid telling us to come back later
            player.entities:gotoAndTrigger('Cid', { eventId = 858 })

            -- Skip to next vanadiel day and progress the mission
            xi.test.world:skipToNextVanaDay()
            player.entities:gotoAndTrigger('Cid', { eventId = 890 })
            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.CALM_BEFORE_THE_STORM)
        end)
    end)

    describe('7-4 Calm Before the Storm', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.CALM_BEFORE_THE_STORM)

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p4')
            local boggelmann = player.entities:get(zones[xi.zone.MISAREAUX_COAST].mob.BOGGELMANN)
            boggelmann.assert:isSpawned()
            player:claimAndKillMob(boggelmann)
            player.entities:gotoAndTrigger('_0p4', { eventId = 13 })
            player.assert:hasKI(xi.ki.VESSEL_OF_LIGHT)

            player:gotoZone(xi.zone.CARPENTERS_LANDING)
            player.entities:gotoAndTrigger('qm_cryptonberries')
            local crypton = player.entities:get(zones[xi.zone.CARPENTERS_LANDING].mob.CRYPTONBERRY_EXECUTOR)
            crypton.assert:isSpawned()
            player:claimAndKillMob(crypton)
            player:setLocalVar(
                string.format('Mission[%d][%d]carpentersNm', xi.mission.log_id.COP,
                    xi.mission.id.cop.CALM_BEFORE_THE_STORM),
                15)
            player.entities:gotoAndTrigger('qm_cryptonberries', { eventId = 37 })

            player:gotoZone(xi.zone.BIBIKI_BAY)
            local dalham = player.entities:get('Dalham')
            player.entities:gotoAndTrigger('qm_dalham')
            dalham.assert:isSpawned()
            player:claimAndKillMob(dalham)
            player.entities:gotoAndTrigger('qm_dalham', { eventId = 41 })

            player:gotoZone(xi.zone.METALWORKS)
            --             player.entities:gotoAndTrigger('Cid', { eventId = 891 })
            player.entities:gotoAndTrigger('Cid', { eventId = 892 })
            player.assert:hasKI(xi.ki.LETTERS_FROM_ULMIA_AND_PRISHE)

            player:gotoZone(xi.zone.SEALIONS_DEN)
            player.entities:gotoAndTrigger('Sueleen', { eventId = 17 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_WARRIORS_PATH)
        end)
    end)

    describe('7-5 The Warriors Path', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_WARRIORS_PATH)

            player:gotoZone(xi.zone.SEALIONS_DEN)
            player.entities:gotoAndTrigger('_0w0', { eventId = 32 })

            player.bcnm:enter('_0w0', xi.battlefield.id.WARRIORS_PATH)
            -- TODO: Tenzen can't be killed and causes this call to fail
            --             player.bcnm:killMobs()
            --             player.bcnm:expectWin({ finishOption = 2 })
            --             player.events:expect({ eventId = 1 })
            --
            --             player.assert:inZone(xi.zone.ALTAIEU)
            --             player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.GARDEN_OF_ANTIQUITY)
        end)
    end)

end)
