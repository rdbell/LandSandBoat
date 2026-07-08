describe('Base entity pet name helper binding', function()
    it('returns the current pet name for battle entities', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        assert(player:getPetName() == '', 'player should not start with a pet name')
        assert(npc:getPetName() == '', 'NPC should fall back to empty pet name')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')

        assert(player:getPetName() == pet:getName(), 'player should report current pet name')
        assert(pet:getPetName() == '', 'pet receiver should not report its own name without a pet')

        player:despawnPet()
        assert(player:getPetName() == '', 'despawned pet should clear pet name')
        assert(not pcall(player.getPetName), 'getPetName accepted missing self')
    end)
end)
