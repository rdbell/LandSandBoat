describe('Base entity minimum pet level helper binding', function()
    it('returns pet list minimum levels for pet entities', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        assert(player:getMinimumPetLevel() == 0, 'non-pet player should return minimum pet level 0')

        player:spawnPet(xi.petId.FIRE_SPIRIT)
        local spirit = player:getPet()
        assert(spirit, 'Fire Spirit was not summoned')
        assert(spirit:getPetID() == xi.petId.FIRE_SPIRIT, 'Fire Spirit pet ID mismatch')
        assert(spirit:getMinimumPetLevel() == 1, 'Fire Spirit minimum pet level mismatch')

        player:despawnPet()
        player:spawnPet(xi.petId.CARBUNCLE)
        local avatar = player:getPet()
        assert(avatar, 'Carbuncle was not summoned')
        assert(avatar:getMinimumPetLevel() == 1, 'Carbuncle minimum pet level mismatch')

        player:despawnPet()
        player:changeJob(xi.job.BST)
        player:spawnPet(xi.petId.SHEEP_FAMILIAR)
        local jug = player:getPet()
        assert(jug, 'Sheep Familiar was not summoned')
        assert(jug:getMinimumPetLevel() == 23, 'Sheep Familiar minimum pet level mismatch')

        assert(not pcall(player.getMinimumPetLevel), 'getMinimumPetLevel accepted missing self')
    end)
end)
