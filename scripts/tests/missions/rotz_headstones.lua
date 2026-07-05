describe('Rise of the Zilart', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer()
    end)

    describe('ZM5 - Headstone Pilgrimage', function()
        before_each(function()
            player:addMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.HEADSTONE_PILGRIMAGE)
        end)

        it('should collect Water fragment', function()
            player:gotoZone(xi.zone.LA_THEINE_PLATEAU)
            player.entities:gotoAndTrigger('Cermet_Headstone', { eventId = 200, finishOption = 1 })
            player.assert:hasKI(xi.ki.WATER_FRAGMENT)
            player.entities:gotoAndTrigger('Cermet_Headstone')
            player.events:expectNotInEvent()
        end)

        it('should collect Ice fragment', function()
            player:gotoZone(xi.zone.CLOISTER_OF_FROST)
            player.entities:gotoAndTrigger('Cermet_Headstone', { eventId = 200, finishOption = 1 })
            player.assert:hasKI(xi.ki.ICE_FRAGMENT)
            player.entities:gotoAndTrigger('Cermet_Headstone')
            player.events:expectNotInEvent()
        end)

        it('should collect Earth fragment', function()
            player:gotoZone(xi.zone.WESTERN_ALTEPA_DESERT)
            player.entities:gotoAndTrigger('Cermet_Headstone', { eventId = 200, finishOption = 1 })
            player.assert:hasKI(xi.ki.EARTH_FRAGMENT)
            player.entities:gotoAndTrigger('Cermet_Headstone')
            player.events:expectNotInEvent()
        end)

        it('should collect Fire fragment by defeating Tipha and Carthi', function()
            player:gotoZone(xi.zone.YUHTUNGA_JUNGLE)
            local tipha = player.entities:get(zones[xi.zone.YUHTUNGA_JUNGLE].mob.TIPHA)
            local carthi = player.entities:get(zones[xi.zone.YUHTUNGA_JUNGLE].mob.CARTHI)
            tipha.assert.no:isSpawned()
            carthi.assert.no:isSpawned()

            -- Ensure repeated test runs don't prevent respawn from cooldown
            local yuhtungaHeadstone = player.entities:get('Cermet_Headstone')
            yuhtungaHeadstone:setLocalVar('cooldown', 0)

            player.entities:gotoAndTrigger(yuhtungaHeadstone, { eventId = 200, finishOption = 1 })
            tipha.assert:isSpawned()
            carthi.assert:isSpawned()

            player:claimAndKillMob(tipha)
            player:claimAndKillMob(carthi)

            player.entities:gotoAndTrigger(yuhtungaHeadstone, { finishOption = 1 })
            player.assert:hasKI(xi.ki.FIRE_FRAGMENT)
            player.events:expectNotInEvent()
            tipha.assert.no:isSpawned()
            carthi.assert.no:isSpawned()
        end)

        it('should collect Wind fragment by defeating Axesarion', function()
            player:gotoZone(xi.zone.CAPE_TERIGGAN)
            local axesarion = player.entities:get(zones[xi.zone.CAPE_TERIGGAN].mob.AXESARION_THE_WANDERER)
            axesarion.assert.no:isSpawned()

            -- Ensure repeated test runs don't prevent respawn from cooldown
            local terigganHeadstone = player.entities:get('Cermet_Headstone')
            terigganHeadstone:setLocalVar('cooldown', 0)

            player.entities:gotoAndTrigger(terigganHeadstone, { eventId = 200, finishOption = 1 })
            axesarion.assert:isSpawned()

            player:claimAndKillMob(axesarion)

            player.entities:gotoAndTrigger(terigganHeadstone, { eventId = 201, finishOption = 1 })
            player.assert:hasKI(xi.ki.WIND_FRAGMENT)
            player.events:expectNotInEvent()
            axesarion.assert.no:isSpawned()
        end)

        it('should collect Lightning fragment by defeating Legendary and Ancient Weapons', function()
            player:gotoZone(xi.zone.BEHEMOTHS_DOMINION)
            local legWeapon = player.entities:get(zones[xi.zone.BEHEMOTHS_DOMINION].mob.LEGENDARY_WEAPON)
            local ancWeapon = player.entities:get(zones[xi.zone.BEHEMOTHS_DOMINION].mob.ANCIENT_WEAPON)
            legWeapon.assert.no:isSpawned()
            ancWeapon.assert.no:isSpawned()

            -- Ensure repeated test runs don't prevent respawn from cooldown
            local bdHeadstone = player.entities:get('Cermet_Headstone')
            bdHeadstone:setLocalVar('cooldown', 0)

            player.entities:gotoAndTrigger(bdHeadstone, { eventId = 200, finishOption = 1 })
            legWeapon.assert:isSpawned()
            ancWeapon.assert:isSpawned()

            player:claimAndKillMob(legWeapon)
            player:claimAndKillMob(ancWeapon)

            player.entities:gotoAndTrigger(bdHeadstone, { eventId = 201, finishOption = 1 })
            player.assert:hasKI(xi.ki.LIGHTNING_FRAGMENT)
            player.events:expectNotInEvent()
            legWeapon.assert.no:isSpawned()
            ancWeapon.assert.no:isSpawned()
        end)

        it('should collect Light fragment by defeating Doomed Pilgrims', function()
            player:gotoZone(xi.zone.THE_SANCTUARY_OF_ZITAH)
            local pilgrim = player.entities:get(zones[xi.zone.THE_SANCTUARY_OF_ZITAH].mob.DOOMED_PILGRIMS)
            pilgrim.assert.no:isSpawned()

            -- Ensure repeated test runs don't prevent respawn from cooldown
            local zitahHeadstone = player.entities:get('Cermet_Headstone')
            zitahHeadstone:setLocalVar('cooldown', 0)

            player.entities:gotoAndTrigger(zitahHeadstone, { eventId = 200, finishOption = 1 })
            pilgrim.assert:isSpawned()

            player:claimAndKillMob(pilgrim)

            player.entities:gotoAndTrigger(zitahHeadstone, { eventId = 201, finishOption = 1 })
            player.assert:hasKI(xi.ki.LIGHT_FRAGMENT)
            player.events:expectNotInEvent()
            pilgrim.assert.no:isSpawned()
        end)

        it('Earth fragment should complete the mission after collecting all fragments', function()
            player:addKeyItem(xi.ki.WATER_FRAGMENT)
            player:addKeyItem(xi.ki.ICE_FRAGMENT)
            player:addKeyItem(xi.ki.FIRE_FRAGMENT)
            player:addKeyItem(xi.ki.WIND_FRAGMENT)
            player:addKeyItem(xi.ki.LIGHTNING_FRAGMENT)
            player:addKeyItem(xi.ki.LIGHT_FRAGMENT)
            player:addKeyItem(xi.ki.DARK_FRAGMENT)
            player:gotoZone(xi.zone.WESTERN_ALTEPA_DESERT)
            player.entities:gotoAndTrigger('Cermet_Headstone', { eventId = 200, finishOption = 1 })
            player.assert:hasKI(xi.ki.EARTH_FRAGMENT)
            player.assert:hasMission(xi.mission.log_id.ZILART, xi.mission.id.zilart.THROUGH_THE_QUICKSAND_CAVES)
        end)
    end)

end)
