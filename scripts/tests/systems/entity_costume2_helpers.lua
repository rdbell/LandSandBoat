describe('Base entity costume2 helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player costume2 while non-player and hidden entities fall back', function()
        local npc = player.entities:get('Field_Manual')

        assert(npc, 'Field Manual NPC was not found')

        assert(player:getCostume2() == 0, 'Player initial costume2 was not zero')
        assert(npc:getCostume2() == 0, 'NPC costume2 fallback was not zero')

        player:setCostume2(0x0123)
        assert(player:getCostume2() == 0x0123, 'Player costume2 was not updated')

        npc:setCostume2(0x0456)
        assert(npc:getCostume2() == 0, 'NPC costume2 changed after setCostume2')

        player:setStatus(xi.status.DISAPPEAR)
        player:setCostume2(0x0789)
        assert(player:getCostume2() == 0x0123, 'Disappear status allowed costume2 update')

        player:setStatus(xi.status.NORMAL)
        player:setCostume2(0)
        assert(player:getCostume2() == 0, 'Player costume2 was not cleared')
    end)
end)
