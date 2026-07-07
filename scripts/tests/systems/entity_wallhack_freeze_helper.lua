describe('Base entity wallhack and freeze helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player wallhack state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getWallhack() == false, 'Player initial wallhack flag was not false')
        assert(npc:getWallhack() == false, 'NPC wallhack fallback was not false')
        assert(mob:getWallhack() == false, 'Mob wallhack fallback was not false')

        player:setWallhack(true)
        assert(player:getWallhack() == true, 'Player wallhack flag was not set')

        player:setWallhack(false)
        assert(player:getWallhack() == false, 'Player wallhack flag was not cleared')

        npc:setWallhack(true)
        mob:setWallhack(true)
        assert(npc:getWallhack() == false, 'NPC wallhack fallback changed after setter')
        assert(mob:getWallhack() == false, 'Mob wallhack fallback changed after setter')
    end)

    it('accepts player freeze transitions and ignores non-players', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setFreezeFlag(true)
        player:setFreezeFlag(false)
        npc:setFreezeFlag(true)
        mob:setFreezeFlag(true)
    end)
end)
