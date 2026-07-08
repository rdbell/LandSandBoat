describe('Base entity hasPet helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('reports visible pets for battle entities and rejects NPCs', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        assert(not player:hasPet(), 'player should not start with a pet')
        assert(not mob:hasPet(), 'mob should not start with a pet')
        assert(not npc:hasPet(), 'NPC should not have pets')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(player:hasPet(), 'player should report visible pet')

        player:despawnPet()
        assert(not player:hasPet(), 'despawned pet should not count')
        assert(not pcall(player.hasPet), 'hasPet accepted missing self')
    end)
end)
