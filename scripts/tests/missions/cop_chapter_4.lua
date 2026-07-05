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

    describe('4-1 Sheltering Doubt', function()
        it('should complete story progression in Tavnazian Safehold', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.SHELTERING_DOUBT)

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            xi.test.world:tick()
            player.events:expect({ event = 107 })

            player.entities:gotoAndTrigger('Justinius')
            player.events:expect({ event = 129 })

            player.entities:gotoAndTrigger('Despachiaire')
            player.events:expect({ event = 108 })

            player.entities:gotoAndTrigger('Justinius')
            player.events:expect({ event = 109 })

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p0')
            player.events:expect({ event = 7 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_SAVAGE)
        end)
    end)

    describe('4-2 The Savage', function()
        it('should complete Monarch Linn BCNM for Savage battle', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_SAVAGE)

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p2', { eventId = 8, finishOption = 1 })

            player:gotoZone(xi.zone.RIVERNE_SITE_B01)

            player:gotoZone(xi.zone.MONARCH_LINN)
            player.bcnm:enter('SD_Entrance', xi.battlefield.id.SAVAGE)
            player.bcnm:killMobs()
            player.bcnm:expectWin({ finishOption = 2 })

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('Justinius', { eventId = 110 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_SECRETS_OF_WORSHIP)
        end)
    end)

    describe('4-3 The Secrets of Worship', function()
        it('should complete the mission successfully', function()
            local ID = zones[xi.zone.SACRARIUM]

            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_SECRETS_OF_WORSHIP)

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD)
            player.entities:gotoAndTrigger('_0qa', { eventId = 111 })

            player:gotoZone(xi.zone.MISAREAUX_COAST)
            player.entities:gotoAndTrigger('_0p8', { eventId = 9, finishOption = 1 })

            -- Player is now in Sacrarium
            player.assert:inZone(xi.zone.SACRARIUM)
            player.entities:gotoAndTrigger('_0s8', { eventId = 6 })

            local qm3 = player.entities:get('qm_prof_3')
            qm3:setLocalVar('hasProfessorMariselle', 1)
            player.entities:gotoAndTrigger('qm_prof_3')
            xi.test.world:tick()

            local professor = player.entities:get(ID.mob.OLD_PROFESSOR_MARISELLE)
            professor.assert:isSpawned()
            player:claimAndKillMob(professor)
            -- Shouldn't need to do these 3 but the test is flaky without them
            professor:despawn()
            xi.test.world:tick()
            qm3:setLocalVar('hasProfessorMariselle', 0)

            player.entities:gotoAndTrigger('qm_prof_3')
            player.assert:hasKI(xi.ki.RELIQUIARIUM_KEY)

            player.entities:gotoAndTrigger('_0s8', { eventId = 5 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.SLANDEROUS_UTTERINGS)
        end)
    end)

    describe('4-4 Slanderous Utterings', function()
        it('should complete the mission successfully', function()
            -- setup mission
            player:addMission(xi.mission.log_id.COP, xi.mission.id.cop.SLANDEROUS_UTTERINGS)

            player:gotoZone(xi.zone.TAVNAZIAN_SAFEHOLD, { x = 106, y = -40, z = -80 })
            xi.test.world:skipTime(1)
            xi.test.world:tick()
            player.events:expect({ eventId = 112 })

            player:gotoZone(xi.zone.SEALIONS_DEN)
            player.entities:gotoAndTrigger('_0w0', { eventId = 13 })

            player.assert:hasMission(xi.mission.log_id.COP, xi.mission.id.cop.THE_ENDURING_TUMULT_OF_WAR)
        end)
    end)

end)
