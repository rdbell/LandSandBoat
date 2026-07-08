describe('Base entity visual packet helper binding', function()
    ---@type CClientEntityPair
    local player

    local function assertVisualPacket(receiver, command, message, ...)
        local ok, result = pcall(receiver.entityVisualPacket, receiver, command, ...)
        assert(ok, message)
        assert(result == nil, message .. ' should return nil')
    end

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('accepts player visual packet commands with optional entities', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assertVisualPacket(player, 'pc00', 'player visual packet rejected nil entity')
        assertVisualPacket(player, 'pc01', 'player visual packet rejected explicit nil entity', nil)
        assertVisualPacket(player, 'pc02', 'player visual packet rejected mob entity', mob)
        assertVisualPacket(player, 'longer', 'player visual packet rejected longer command', npc)
        assertVisualPacket(mob, 'mb00', 'mob visual packet rejected no-op call')
        assertVisualPacket(npc, 'np00', 'NPC visual packet rejected no-op call')

        assert(not pcall(player.entityVisualPacket), 'entityVisualPacket accepted missing self')
        assert(not pcall(player.entityVisualPacket, player), 'entityVisualPacket accepted missing command')
        assert(not pcall(player.entityVisualPacket, player, {}), 'entityVisualPacket accepted non-string command')
        assert(not pcall(player.entityVisualPacket, player, 'bad', {}), 'entityVisualPacket accepted non-entity target')
        assert(pcall(mob.entityVisualPacket, mob, 'mb01', {}), 'non-player entityVisualPacket validated target after no-op')
    end)
end)
