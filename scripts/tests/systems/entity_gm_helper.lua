describe('Base entity GM helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player GM state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getGMLevel() == 0, 'Player initial GM level was not zero')
        assert(player:getVisibleGMLevel() == 0, 'Player initial visible GM level was not zero')
        assert(player:getGMHidden() == false, 'Player initial GM hidden flag was not false')
        assert(npc:getGMLevel() == 0, 'NPC GM level fallback was not zero')
        assert(npc:getVisibleGMLevel() == 0, 'NPC visible GM level fallback was not zero')
        assert(npc:getGMHidden() == false, 'NPC GM hidden fallback was not false')
        assert(mob:getGMLevel() == 0, 'Mob GM level fallback was not zero')
        assert(mob:getVisibleGMLevel() == 0, 'Mob visible GM level fallback was not zero')
        assert(mob:getGMHidden() == false, 'Mob GM hidden fallback was not false')

        player:setGMLevel(3)
        player:setVisibleGMLevel(4)
        player:setGMHidden(true)
        assert(player:getGMLevel() == 3, 'Player GM level was not set')
        assert(player:getVisibleGMLevel() == 4, 'Player visible GM level was not set')
        assert(player:getGMHidden() == true, 'Player GM hidden flag was not set')

        player:setGMLevel(0)
        player:setVisibleGMLevel(0)
        player:setGMHidden(false)
        assert(player:getGMLevel() == 0, 'Player GM level was not cleared')
        assert(player:getVisibleGMLevel() == 0, 'Player visible GM level was not cleared')
        assert(player:getGMHidden() == false, 'Player GM hidden flag was not cleared')

        npc:setGMLevel(1)
        npc:setVisibleGMLevel(1)
        npc:setGMHidden(true)
        mob:setGMLevel(1)
        mob:setVisibleGMLevel(1)
        mob:setGMHidden(true)
        assert(npc:getGMLevel() == 0, 'NPC GM level fallback changed after setter')
        assert(npc:getVisibleGMLevel() == 0, 'NPC visible GM level fallback changed after setter')
        assert(npc:getGMHidden() == false, 'NPC GM hidden fallback changed after setter')
        assert(mob:getGMLevel() == 0, 'Mob GM level fallback changed after setter')
        assert(mob:getVisibleGMLevel() == 0, 'Mob visible GM level fallback changed after setter')
        assert(mob:getGMHidden() == false, 'Mob GM hidden fallback changed after setter')
    end)
end)
