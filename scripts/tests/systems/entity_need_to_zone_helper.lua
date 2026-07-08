describe('Base entity need-to-zone helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('stores and reads the generic must-zone character variable for players', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setCharVar('[generic]mustZone', 0)
        assert(player:needToZone() == false, 'cleared player unexpectedly needed to zone')
        assert(player:needToZone(false) == false, 'false write argument did not read cleared state')

        assert(player:needToZone(true) == true, 'true write argument did not return true')
        assert(player:getCharVar('[generic]mustZone') == xi.zone.WEST_RONFAURE, 'needToZone did not store current zone')
        assert(player:needToZone() == true, 'stored must-zone flag was not readable')
        assert(player:needToZone(false) == true, 'false write argument cleared must-zone state')

        player:setCharVar('[generic]mustZone', 0)
        assert(player:needToZone() == false, 'cleared must-zone flag was still readable')

        assert(npc:needToZone() == false, 'NPC needToZone fallback was not false')
        assert(mob:needToZone() == false, 'mob needToZone fallback was not false')
        assert(npc:needToZone(true) == false, 'NPC needToZone write fallback was not false')
        assert(mob:needToZone(true) == false, 'mob needToZone write fallback was not false')

        assert(not pcall(player.needToZone), 'needToZone accepted missing self')
    end)
end)
