describe('Base entity target id helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads stable zone-local target ids for real entities', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        local playerTargID = player:getTargID()
        local npcTargID    = npc:getTargID()
        local mobTargID    = mob:getTargID()

        assert(type(playerTargID) == 'number', 'Player target ID was not numeric')
        assert(type(npcTargID) == 'number', 'NPC target ID was not numeric')
        assert(type(mobTargID) == 'number', 'Mob target ID was not numeric')

        assert(playerTargID > 0, 'Player target ID was empty')
        assert(npcTargID > 0, 'NPC target ID was empty')
        assert(mobTargID > 0, 'Mob target ID was empty')

        assert(player:getTargID() == playerTargID, 'Player target ID changed')
        assert(npc:getTargID() == npcTargID, 'NPC target ID changed')
        assert(mob:getTargID() == mobTargID, 'Mob target ID changed')
    end)
end)
