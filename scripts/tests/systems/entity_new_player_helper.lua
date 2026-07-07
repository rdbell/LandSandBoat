describe('Base entity new player helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player new-player state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getNewPlayer() == true, 'Player initial new-player flag was not true')
        assert(npc:getNewPlayer() == false, 'NPC new-player fallback was not false')
        assert(mob:getNewPlayer() == false, 'Mob new-player fallback was not false')

        player:setNewPlayer(false)
        assert(player:getNewPlayer() == false, 'Player new-player flag was not cleared')

        player:setNewPlayer(true)
        assert(player:getNewPlayer() == true, 'Player new-player flag was not restored')

        npc:setNewPlayer(false)
        mob:setNewPlayer(false)
        assert(npc:getNewPlayer() == false, 'NPC new-player fallback changed after setter')
        assert(mob:getNewPlayer() == false, 'Mob new-player fallback changed after setter')
    end)
end)
