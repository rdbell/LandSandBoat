describe('Base entity speed helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates base speed for players and non-player entities', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getSpeed() == xi.settings.map.BASE_SPEED, 'Player initial speed did not match BASE_SPEED')
        assert(player:getBaseSpeed() == xi.settings.map.BASE_SPEED, 'Player initial base speed did not match BASE_SPEED')
        assert(npc:getBaseSpeed() == xi.settings.map.BASE_SPEED, 'NPC initial base speed did not match BASE_SPEED')
        assert(mob:getBaseSpeed() > 0, 'Mob initial base speed was not positive')

        player:setBaseSpeed(37)
        npc:setBaseSpeed(0)
        mob:setBaseSpeed(23)
        assert(player:getSpeed() == 37, 'Player current speed was not updated from base speed')
        assert(player:getBaseSpeed() == 37, 'Player base speed was not set')
        assert(npc:getSpeed() == 0, 'NPC current speed was not updated to zero')
        assert(npc:getBaseSpeed() == 0, 'NPC base speed was not set to zero')
        assert(mob:getSpeed() == 23, 'Mob current speed was not updated from base speed')
        assert(mob:getBaseSpeed() == 23, 'Mob base speed was not set')

        player:setBaseSpeed(256)
        assert(player:getSpeed() == 0, 'Player speed did not wrap uint8 base speed input')
        assert(player:getBaseSpeed() == 0, 'Player base speed did not wrap uint8 input')

        player:setBaseSpeed(50)
        npc:setBaseSpeed(50)
        mob:setBaseSpeed(50)
    end)

    it('accepts animation speed updates for players and non-player entities', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setAnimationSpeed(33)
        npc:setAnimationSpeed(0)
        mob:setAnimationSpeed(88)
        player:setAnimationSpeed(50)
        npc:setAnimationSpeed(50)
        mob:setAnimationSpeed(50)
    end)
end)
