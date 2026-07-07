describe('Base entity allegiance helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads default allegiances and updates spawned player allegiance', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getAllegiance() == xi.allegiance.PLAYER, 'Player allegiance was not PLAYER')
        assert(npc:getAllegiance() == xi.allegiance.MOB, 'NPC allegiance was not MOB')
        assert(mob:getAllegiance() == xi.allegiance.MOB, 'Mob allegiance was not MOB')

        player:setAllegiance(xi.allegiance.SAN_DORIA)
        assert(player:getAllegiance() == xi.allegiance.SAN_DORIA, 'Player allegiance did not update')

        player:setAllegiance(xi.allegiance.PLAYER)
        assert(player:getAllegiance() == xi.allegiance.PLAYER, 'Player allegiance did not restore')
    end)
end)
