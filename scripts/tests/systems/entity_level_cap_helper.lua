describe('Base entity level cap helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player level cap while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setLevelCap(55)
        assert(player:getLevelCap() == 55, 'Player level cap was not set')

        player:setLevelCap(280)
        assert(player:getLevelCap() == 24, 'Player level cap did not wrap uint8 input')

        assert(npc:getLevelCap() == 0, 'NPC level cap fallback was not zero')
        assert(mob:getLevelCap() == 0, 'Mob level cap fallback was not zero')

        npc:setLevelCap(75)
        mob:setLevelCap(75)
        assert(npc:getLevelCap() == 0, 'NPC level cap fallback changed after setter')
        assert(mob:getLevelCap() == 0, 'Mob level cap fallback changed after setter')
    end)
end)
