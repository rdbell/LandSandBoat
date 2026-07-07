describe('Base entity costume helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player costumes while non-player and hidden entities fall back', function()
        local npc = player.entities:get('Field_Manual')

        assert(npc, 'Field Manual NPC was not found')

        assert(player:getCostume() == 0, 'Player initial costume was not zero')
        assert(npc:getCostume() == 0, 'NPC costume fallback was not zero')

        player:setCostume(0x0123)
        assert(player:getCostume() == 0x0123, 'Player costume was not updated')

        npc:setCostume(0x0456)
        assert(npc:getCostume() == 0, 'NPC costume changed after setCostume')

        player:setStatus(xi.status.DISAPPEAR)
        player:setCostume(0x0789)
        assert(player:getCostume() == 0x0123, 'Disappear status allowed costume update')

        player:setStatus(xi.status.NORMAL)
        player:setCostume(0)
        assert(player:getCostume() == 0, 'Player costume was not cleared')
    end)
end)
