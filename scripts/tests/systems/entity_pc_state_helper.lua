describe('Base entity PC state helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates PC-only state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        local lastOnline = player:getLastOnline()
        assert(lastOnline > 0, 'new test player last-online timestamp was not positive')
        assert(lastOnline <= GetSystemTime(), 'new test player last-online timestamp was in the future')
        assert(npc:getLastOnline() == 0, 'NPC last-online fallback was not zero')
        assert(mob:getLastOnline() == 0, 'mob last-online fallback was not zero')

        assert(player:didGetMessage() == false, 'new test player got-message flag was not false')
        player:resetGotMessage()
        assert(player:didGetMessage() == false, 'resetGotMessage did not clear got-message flag')
        npc:resetGotMessage()
        mob:resetGotMessage()
        assert(npc:didGetMessage() == false, 'NPC got-message fallback was not false')
        assert(mob:didGetMessage() == false, 'mob got-message fallback was not false')

        assert(player:getMoghouseFlag() == 0, 'new test player Mog House flag was not zero')
        player:setMoghouseFlag(0x1234)
        assert(player:getMoghouseFlag() == 0x1234, 'setMoghouseFlag did not store the flag')
        npc:setMoghouseFlag(0xFFFF)
        mob:setMoghouseFlag(0xFFFF)
        assert(npc:getMoghouseFlag() == 0, 'NPC Mog House flag fallback was not zero')
        assert(mob:getMoghouseFlag() == 0, 'mob Mog House flag fallback was not zero')

        assert(player:hasVisitedZone(xi.zone.WEST_RONFAURE) == true, 'current zone was not marked visited')
        assert(player:hasVisitedZone(xi.zone.BIBIKI_BAY) == false, 'unvisited zone was marked visited')
        assert(npc:hasVisitedZone(xi.zone.WEST_RONFAURE) == false, 'NPC visited-zone fallback was not false')
        assert(mob:hasVisitedZone(xi.zone.WEST_RONFAURE) == false, 'mob visited-zone fallback was not false')

        assert(player:inMogHouse() == false, 'West Ronfaure player was reported in Mog House')
        assert(npc:inMogHouse() == false, 'NPC Mog House fallback was not false')
        assert(mob:inMogHouse() == false, 'mob Mog House fallback was not false')

        assert(not pcall(function()
            player:setMoghouseFlag()
        end), 'setMoghouseFlag accepted missing flag')

        assert(not pcall(function()
            player:hasVisitedZone()
        end), 'hasVisitedZone accepted missing zone')
    end)
end)
