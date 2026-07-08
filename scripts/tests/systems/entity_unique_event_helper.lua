describe('Base entity unique event helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player unique event bits while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:hasCompletedUniqueEvent(0) == false, 'Initial event 0 unexpectedly completed')
        assert(player:hasCompletedUniqueEvent(31) == false, 'Initial event 31 unexpectedly completed')
        assert(player:hasCompletedUniqueEvent(32) == false, 'Initial event 32 unexpectedly completed')
        assert(player:hasCompletedUniqueEvent(159) == false, 'Initial event 159 unexpectedly completed')

        player:setUniqueEvent(0)
        player:setUniqueEvent(31)
        player:setUniqueEvent(32)
        player:setUniqueEvent(159)
        assert(player:hasCompletedUniqueEvent(0) == true, 'setUniqueEvent did not set bit 0')
        assert(player:hasCompletedUniqueEvent(31) == true, 'setUniqueEvent did not set bit 31')
        assert(player:hasCompletedUniqueEvent(32) == true, 'setUniqueEvent did not set bit 32')
        assert(player:hasCompletedUniqueEvent(159) == true, 'setUniqueEvent did not set bit 159')

        player:setUniqueEvent(31)
        assert(player:hasCompletedUniqueEvent(31) == true, 'Repeated setUniqueEvent cleared bit 31')

        player:delUniqueEvent(31)
        assert(player:hasCompletedUniqueEvent(31) == false, 'delUniqueEvent did not clear bit 31')
        assert(player:hasCompletedUniqueEvent(32) == true, 'delUniqueEvent cleared adjacent word bit')

        player:delUniqueEvent(31)
        assert(player:hasCompletedUniqueEvent(31) == false, 'Repeated delUniqueEvent set bit 31')

        player:setUniqueEvent(65537)
        assert(player:hasCompletedUniqueEvent(1) == true, 'Unique event ID did not wrap through uint16 input')
        player:delUniqueEvent(65537)
        assert(player:hasCompletedUniqueEvent(1) == false, 'Wrapped unique event delete did not clear bit 1')

        npc:setUniqueEvent(5)
        mob:setUniqueEvent(5)
        assert(npc:hasCompletedUniqueEvent(5) == false, 'NPC unique event fallback changed after setter')
        assert(mob:hasCompletedUniqueEvent(5) == false, 'Mob unique event fallback changed after setter')

        npc:delUniqueEvent(5)
        mob:delUniqueEvent(5)
        assert(npc:hasCompletedUniqueEvent(5) == false, 'NPC unique event fallback changed after delete')
        assert(mob:hasCompletedUniqueEvent(5) == false, 'Mob unique event fallback changed after delete')
    end)
end)
