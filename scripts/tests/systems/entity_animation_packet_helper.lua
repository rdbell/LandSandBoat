describe('Base entity animation packet helper binding', function()
    ---@type CClientEntityPair
    local player

    local function assertAnimationPacket(receiver, command, message, ...)
        local ok, result = pcall(receiver.entityAnimationPacket, receiver, command, ...)
        assert(ok, message)
        assert(result == nil, message .. ' should return nil')
    end

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('accepts entity animation commands with default and explicit targets', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assertAnimationPacket(player, 'pc00', 'player animation packet rejected default target')
        assertAnimationPacket(player, 'pc01', 'player animation packet rejected mob target', mob)
        assertAnimationPacket(player, 'pc02', 'player animation packet rejected nil target', nil)
        assertAnimationPacket(player, 'longer', 'player animation packet rejected longer command', mob)
        assertAnimationPacket(mob, 'mb00', 'mob animation packet rejected default target')
        assertAnimationPacket(mob, 'mb01', 'mob animation packet rejected player target', player)
        assertAnimationPacket(npc, 'np00', 'NPC animation packet rejected default target')
        assertAnimationPacket(npc, 'np01', 'NPC animation packet rejected player target', player)

        assert(not pcall(player.entityAnimationPacket), 'entityAnimationPacket accepted missing self')
        assert(not pcall(player.entityAnimationPacket, player), 'entityAnimationPacket accepted missing command')
        assert(not pcall(player.entityAnimationPacket, player, {}), 'entityAnimationPacket accepted non-string command')
        assert(not pcall(player.entityAnimationPacket, player, 'bad', {}), 'entityAnimationPacket accepted non-entity target')
    end)
end)
