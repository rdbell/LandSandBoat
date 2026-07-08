describe('Base entity pet modifier helper bindings', function()
    it('adds, sets, and deletes modifiers on the current pet', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        player:addPetMod(xi.mod.ATT, 99)
        assert(player:getPet() == nil, 'player should not start with a pet')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')

        local baseAttack = pet:getMod(xi.mod.ATT)

        player:addPetMod(xi.mod.ATT, 25)
        player:addPetMod(xi.mod.ATT, -5)
        assert(pet:getMod(xi.mod.ATT) == baseAttack + 20, 'addPetMod did not add signed amounts')
        assert(player:getMod(xi.mod.ATT) == 0, 'addPetMod should not mutate owner modifiers')

        player:setPetMod(xi.mod.ATT, 7)
        assert(pet:getMod(xi.mod.ATT) == 7, 'setPetMod did not set pet modifier amount')

        player:delPetMod(xi.mod.ATT, 4)
        player:delPetMod(xi.mod.ATT, -2)
        assert(pet:getMod(xi.mod.ATT) == 5, 'delPetMod did not delete signed amounts')

        npc:addPetMod(xi.mod.ATT, 100)
        npc:setPetMod(xi.mod.ATT, 100)
        npc:delPetMod(xi.mod.ATT, 100)
        assert(pet:getMod(xi.mod.ATT) == 5, 'NPC pet mod helpers should be ignored')

        player:despawnPet()
        player:addPetMod(xi.mod.DOUBLE_ATTACK, 8)
        player:spawnPet(xi.petId.CARBUNCLE)
        local nextPet = player:getPet()
        assert(nextPet, 'second Carbuncle was not summoned')
        assert(nextPet:getMod(xi.mod.DOUBLE_ATTACK) == 8, 'stored pet modifiers were not applied to the next pet')

        assert(not pcall(player.addPetMod), 'addPetMod accepted missing self')
        assert(not pcall(player.setPetMod), 'setPetMod accepted missing self')
        assert(not pcall(player.delPetMod), 'delPetMod accepted missing self')
    end)
end)
