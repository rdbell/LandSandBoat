describe('Base entity TP helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates battle entity TP while NPCs fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setTP(0)
        assert(player:getTP() == 0, 'setTP did not clear player TP')

        assert(player:addTP(1200) == 1200, 'addTP did not report gained player TP')
        assert(player:getTP() == 1200, 'addTP did not increase player TP')

        assert(player:addTP(2500) == 1800, 'addTP did not report capped player TP gain')
        assert(player:getTP() == 3000, 'addTP did not cap player TP at 3000')

        assert(player:delTP(700) == 700, 'delTP did not report spent player TP')
        assert(player:getTP() == 2300, 'delTP did not decrease player TP')

        assert(player:delTP(4000) == 2300, 'delTP did not report capped player TP loss')
        assert(player:getTP() == 0, 'delTP did not floor player TP at zero')

        player:setTP(3333)
        assert(player:getTP() == 3333, 'setTP should write raw player TP without clamping')
        assert(player:addTP(1) == 333, 'addTP did not report normalization from above cap')
        assert(player:getTP() == 3000, 'addTP did not normalize player TP back to cap')

        mob:setTP(900)
        assert(mob:getTP() == 900, 'setTP did not update mob TP')
        assert(mob:addTP(300) == 300, 'addTP did not report gained mob TP')
        assert(mob:getTP() == 1200, 'addTP did not increase mob TP')
        assert(mob:delTP(200) == 200, 'delTP did not report spent mob TP')
        assert(mob:getTP() == 1000, 'delTP did not decrease mob TP')

        assert(npc:getTP() == 0, 'NPC getTP should fall back to zero')
        assert(npc:addTP(100) == 0, 'NPC addTP should fall back to zero')
        assert(npc:delTP(100) == 0, 'NPC delTP should fall back to zero')
        assert(pcall(npc.setTP, npc, 100), 'NPC setTP rejected a valid no-op')
        assert(npc:getTP() == 0, 'NPC setTP should not update TP')

        assert(not pcall(player.getTP), 'getTP accepted missing self')
        assert(not pcall(player.setTP), 'setTP accepted missing self')
        assert(not pcall(player.setTP, player), 'setTP accepted missing TP')
        assert(not pcall(player.setTP, player, 'bad'), 'setTP accepted non-numeric TP')
        assert(not pcall(player.addTP), 'addTP accepted missing self')
        assert(not pcall(player.addTP, player), 'addTP accepted missing TP')
        assert(not pcall(player.addTP, player, 'bad'), 'addTP accepted non-numeric TP')
        assert(not pcall(player.delTP), 'delTP accepted missing self')
        assert(not pcall(player.delTP, player), 'delTP accepted missing TP')
        assert(not pcall(player.delTP, player, 'bad'), 'delTP accepted non-numeric TP')
    end)
end)
