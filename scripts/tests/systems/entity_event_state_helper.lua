describe('Base entity event state helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates PC event state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:isInEvent() == false, 'new test player started in an event')
        assert(npc:isInEvent() == false, 'NPC event fallback was not false')
        assert(mob:isInEvent() == false, 'mob event fallback was not false')

        assert(player:startSequence() == true, 'PC startSequence did not return true')
        assert(npc:startSequence() == false, 'NPC startSequence fallback was not false')
        assert(mob:startSequence() == false, 'mob startSequence fallback was not false')
        assert(player:isInEvent() == false, 'startSequence unexpectedly started an event')
        player:release()
        assert(player:isInEvent() == false, 'release after sequence left player in an event')

        assert(player:getEventTarget() == nil, 'player without event returned an event target')
        assert(npc:getEventTarget() == nil, 'NPC event-target fallback was not nil')
        assert(mob:getEventTarget() == nil, 'mob event-target fallback was not nil')

        player.entities:gotoAndTrigger('Field_Manual')
        assert(player:isInEvent() == true, 'triggered player was not in an event')

        local target = player:getEventTarget()
        assert(target ~= nil, 'triggered event target was nil')
        assert(target:getID() == npc:getID(), 'triggered event target did not match NPC')

        player:release()
        assert(player:isInEvent() == false, 'release did not end the current event')
        assert(player:getEventTarget() == nil, 'released player still returned an event target')

        npc:release()
        mob:release()

        assert(not pcall(player.isInEvent), 'isInEvent accepted missing self')
        assert(not pcall(player.startSequence), 'startSequence accepted missing self')
        assert(not pcall(player.release), 'release accepted missing self')
        assert(not pcall(player.getEventTarget), 'getEventTarget accepted missing self')
    end)
end)
