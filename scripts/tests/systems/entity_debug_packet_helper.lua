describe('Base entity debug packet helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('sends raw packet bytes only for players', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')

        player.packets:clear()
        local result = player:sendDebugPacket({ 0xFE, 0x05, 0x34, 0x12, 0xAB, 0xCD, 0x100, 0x00 })
        assert(result == nil, 'sendDebugPacket should return nil')

        local packets = player.packets:getIncoming()
        assert(#packets == 1, string.format('expected one debug packet, got %d', #packets))
        assert(packets[1].type == 0x1FE, string.format('expected packet type 0x1FE, got 0x%03X', packets[1].type))
        assert(packets[1].size == 8, string.format('expected packet size 8, got %d', packets[1].size))
        assert(packets[1].data[0] == 0xFE, 'debug packet byte 0 mismatch')
        assert(packets[1].data[1] == 0x05, 'debug packet byte 1 mismatch')
        assert(packets[1].data[2] == 0x34, 'debug packet byte 2 mismatch')
        assert(packets[1].data[3] == 0x12, 'debug packet byte 3 mismatch')
        assert(packets[1].data[4] == 0xAB, 'debug packet byte 4 mismatch')
        assert(packets[1].data[5] == 0xCD, 'debug packet byte 5 mismatch')
        assert(packets[1].data[6] == 0x00, 'debug packet byte 6 mismatch')
        assert(packets[1].data[7] == 0x00, 'debug packet byte 7 mismatch')

        player.packets:clear()
        assert(pcall(mob.sendDebugPacket, mob, { 0xFE, 0x05, 0x34, 0x12, 0xAB, 0xCD, 0x7E, 0x00 }),
            'non-player sendDebugPacket rejected a valid no-op table')
        assert(pcall(mob.sendDebugPacket, mob, { 0xFE, 'bad' }),
            'non-player sendDebugPacket validated packet bytes after no-op')
        assert(#player.packets:getIncoming() == 0, 'non-player sendDebugPacket emitted a packet')

        assert(not pcall(player.sendDebugPacket), 'sendDebugPacket accepted missing self')
        assert(not pcall(player.sendDebugPacket, player), 'sendDebugPacket accepted missing packet data')
        assert(not pcall(player.sendDebugPacket, player, 'bad'), 'sendDebugPacket accepted non-table packet data')
        assert(not pcall(player.sendDebugPacket, player, { 0xFE, 'bad' }), 'sendDebugPacket accepted non-numeric packet byte')
    end)
end)
