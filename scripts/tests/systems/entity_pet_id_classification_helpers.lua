describe('Base entity pet ID and classification helper bindings', function()
    it('reads pet IDs and classifies avatar and jug pets', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        assert(player:getPetID() == 0, 'non-pet player should return pet ID 0')
        assert(not player:isAvatar(), 'non-pet player should not be an avatar')
        assert(not player:isJugPet(), 'non-pet player should not be a jug pet')
        assert(not player:isAutomaton(), 'non-pet player should not be an automaton')

        player:spawnPet(xi.petId.CARBUNCLE)
        local avatar = player:getPet()
        assert(avatar, 'Carbuncle was not summoned')
        assert(avatar:getPetID() == xi.petId.CARBUNCLE, 'avatar pet ID mismatch')
        assert(avatar:isAvatar(), 'Carbuncle should be classified as avatar')
        assert(not avatar:isJugPet(), 'Carbuncle should not be classified as jug pet')
        assert(not avatar:isAutomaton(), 'Carbuncle should not be classified as automaton')

        player:despawnPet()
        player:changeJob(xi.job.BST)
        player:spawnPet(xi.petId.SHEEP_FAMILIAR)
        local jug = player:getPet()
        assert(jug, 'Sheep Familiar was not summoned')
        assert(jug:getPetID() == xi.petId.SHEEP_FAMILIAR, 'jug pet ID mismatch')
        assert(jug:isJugPet(), 'Sheep Familiar should be classified as jug pet')
        assert(not jug:isAvatar(), 'Sheep Familiar should not be classified as avatar')
        assert(not jug:isAutomaton(), 'Sheep Familiar should not be classified as automaton')

        assert(not pcall(player.getPetID), 'getPetID accepted missing self')
        assert(not pcall(player.isAvatar), 'isAvatar accepted missing self')
        assert(not pcall(player.isJugPet), 'isJugPet accepted missing self')
        assert(not pcall(player.isAutomaton), 'isAutomaton accepted missing self')
    end)

    it('classifies automaton pet IDs', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.SOUTHERN_SAN_DORIA,
            job   = xi.job.PUP,
            level = 75,
        })

        player:setPetName(xi.petType.AUTOMATON, xi.petName.MK_IV)
        player:unlockAttachment(xi.item.HARLEQUIN_FRAME)
        player:unlockAttachment(xi.item.HARLEQUIN_HEAD)
        player:spawnPet(xi.petId.AUTOMATON)

        local automaton = player:getPet()
        assert(automaton, 'Automaton was not summoned')
        assert(automaton:getPetID() >= xi.petId.AUTOMATON, 'automaton pet ID below automaton range')
        assert(automaton:isAutomaton(), 'Automaton should be classified as automaton')
        assert(not automaton:isAvatar(), 'Automaton should not be classified as avatar')
        assert(not automaton:isJugPet(), 'Automaton should not be classified as jug pet')
    end)
end)
