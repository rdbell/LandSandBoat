describe('Base entity pet element helper binding', function()
    it('returns the current pet element for battle entities', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        assert(player:getPetElement() == 0, 'player should not start with a pet element')
        assert(npc:getPetElement() == 0, 'NPC should fall back to element 0')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')

        assert(player:getPetElement() == xi.element.LIGHT, 'player should report Carbuncle light element')
        assert(pet:getPetElement() == 0, 'pet receiver should not report its own element without a pet')

        player:despawnPet()
        assert(player:getPetElement() == 0, 'despawned pet should clear pet element')
        assert(not pcall(player.getPetElement), 'getPetElement accepted missing self')
    end)
end)
