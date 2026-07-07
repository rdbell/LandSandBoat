describe('Base entity model ID helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads model IDs from player, NPC, and mob look data', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getModelId() == 0x0100, 'Player model ID was not Hume male Face1A')
        assert(npc:getModelId() == 0x08F2, 'Field Manual model ID was not 0x08F2')
        assert(mob:getModelId() == 0x010C, 'Wild Rabbit model ID was not 0x010C')
    end)
end)
