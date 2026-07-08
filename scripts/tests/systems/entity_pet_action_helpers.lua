describe('Base entity pet action helper bindings', function()
    it('orders pets to attack and retreat', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        local target = player.entities:moveTo('Wild_Rabbit')
        assert(target, 'Wild Rabbit mob was not found')
        target:spawn()

        player:petAttack(target)
        assert(player:getPet() == nil, 'player should not gain a pet from petAttack')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')

        player:petAttack(target)
        xi.test.world:tickEntity(pet)
        assert(pet:isEngaged(), 'petAttack should engage the pet')

        player:petRetreat()
        xi.test.world:tickEntity(pet)
        assert(not pet:isEngaged(), 'petRetreat should disengage the pet')

        pet:addStatusEffect(xi.effect.SLEEP_I, { power = 1, duration = 60, origin = pet })
        player:petAttack(target)
        xi.test.world:tickEntity(pet)
        assert(not pet:isEngaged(), 'petAttack should be blocked while the pet is asleep')
        assert(pet:delStatusEffect(xi.effect.SLEEP_I), 'sleep effect should be removed')

        pet:addStatusEffect(xi.effect.CHARM_I, { power = 1, duration = 60, origin = pet })
        player:petAttack(target)
        xi.test.world:tickEntity(pet)
        assert(not pet:isEngaged(), 'petAttack should be blocked while the pet is charmed')

        player:despawnPet()
        player:petRetreat()

        assert(pcall(pet.extendCharm, pet, 15, 15), 'extendCharm should accept a battle entity receiver')
        assert(pcall(pet.extendCharm, pet, 30, 15), 'extendCharm should accept invalid ranges as a no-op')
        assert(pcall(pet.extendCharm, pet, 15, 0), 'extendCharm should accept zero max as a no-op')

        assert(not pcall(player.petAttack), 'petAttack accepted missing self')
        assert(not pcall(player.petAttack, player), 'petAttack accepted missing target')
        assert(not pcall(player.petRetreat), 'petRetreat accepted missing self')
        assert(not pcall(pet.extendCharm), 'extendCharm accepted missing self')
        assert(not pcall(pet.extendCharm, pet), 'extendCharm accepted missing min seconds')
    end)
end)
