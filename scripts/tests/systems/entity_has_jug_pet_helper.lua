describe('Base entity hasJugPet helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.BST,
            level = 99,
        })
    end)

    it('reports visible jug pets only for players', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        assert(not player:hasJugPet(), 'player should not start with a jug pet')
        assert(not mob:hasJugPet(), 'mob should not have jug pets')
        assert(not npc:hasJugPet(), 'NPC should not have jug pets')

        player:spawnPet(xi.petId.CARBUNCLE)
        assert(player:getPet(), 'Carbuncle was not summoned')
        assert(not player:hasJugPet(), 'avatar should not count as jug pet')

        player:despawnPet()
        player:spawnPet(xi.petId.SHEEP_FAMILIAR)
        assert(player:getPet(), 'Sheep Familiar was not summoned')
        assert(player:hasJugPet(), 'player should report visible jug pet')

        player:despawnPet()
        assert(not player:hasJugPet(), 'despawned jug pet should not count')
        assert(not pcall(player.hasJugPet), 'hasJugPet accepted missing self')
    end)
end)
