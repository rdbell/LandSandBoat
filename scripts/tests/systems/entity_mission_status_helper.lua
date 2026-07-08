describe('Base entity mission status helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates packed mission status while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local log = xi.mission.log_id.TOAU

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getMissionStatus(log) == 0, 'Initial mission status was not zero')
        assert(player:getMissionStatus(log, 0) == 0, 'Initial mission status nibble was not zero')

        player:setMissionStatus(log, 0x12345678)
        assert(player:getMissionStatus(log) == 0x12345678, 'Full mission status did not round trip')
        assert(player:getMissionStatus(log, 0) == 0x8, 'Mission status position 0 did not read low nibble')
        assert(player:getMissionStatus(log, 1) == 0x7, 'Mission status position 1 did not read second nibble')
        assert(player:getMissionStatus(log, 4) == 0x4, 'Mission status position 4 did not read upper half')
        assert(player:getMissionStatus(log, 7) == 0x1, 'Mission status position 7 did not read high nibble')

        player:setMissionStatus(log, 0xA, 2)
        assert(player:getMissionStatus(log) == 0x12345A78, 'Indexed mission status write did not update nibble 2')

        player:setMissionStatus(log, 0, 7)
        assert(player:getMissionStatus(log) == 0x02345A78, 'Indexed mission status write did not update high nibble')

        player:setMissionStatus(log, 0xF, 8)
        assert(player:getMissionStatus(log) == 0x02345A78, 'Out-of-range indexed status position changed status')
        assert(player:getMissionStatus(log, 8) == 0, 'Out-of-range indexed status read did not return zero')

        player:setMissionStatus(log, 0x10, 1)
        assert(player:getMissionStatus(log) == 0x02345A78, 'Out-of-range indexed status value changed status')

        player:setMissionStatus(log, 9, 'bad')
        assert(player:getMissionStatus(log) == 9, 'Non-numeric status position did not perform full status write')
        assert(player:getMissionStatus(log, 'bad') == 9, 'Non-numeric status position did not read full status')

        player:setMissionStatus(log, 0xFFFFFFFF)
        assert(player:getMissionStatus(log) == 0xFFFFFFFF, 'Full uint32 mission status did not round trip')
        assert(player:getMissionStatus(log, 7) == 0xF, 'Full uint32 mission status high nibble did not read')

        player:setMissionStatus(255, 0x1234)
        assert(player:getMissionStatus(255) == 0, 'Invalid mission log did not read as zero')
        assert(player:getMissionStatus(log) == 0xFFFFFFFF, 'Invalid mission log write changed valid log status')

        assert(pcall(player.sendPartialMissionLog, player, log, false), 'sendPartialMissionLog valid call errored')
        assert(pcall(player.sendPartialMissionLog, player, 255, true), 'sendPartialMissionLog invalid log call errored')

        npc:setMissionStatus(log, 0x1234)
        mob:setMissionStatus(log, 0x1234)
        npc:sendPartialMissionLog(log, false)
        mob:sendPartialMissionLog(log, true)
        assert(npc:getMissionStatus(log) == 0, 'NPC mission status fallback changed after setters')
        assert(mob:getMissionStatus(log) == 0, 'Mob mission status fallback changed after setters')
    end)
end)
