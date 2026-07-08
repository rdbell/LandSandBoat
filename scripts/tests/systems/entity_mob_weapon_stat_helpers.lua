describe('Base entity mob weapon stat helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('updates mob delay and mob or pet weapon damage while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        local initialDelay = mob:getBaseDelay()
        assert(type(initialDelay) == 'number', 'expected mob base delay to be numeric')
        assert(pcall(mob.setDelay, mob, 240), 'setDelay rejected a valid mob update')
        assert(mob:getBaseDelay() == 240, 'expected mob base delay to update')

        assert(pcall(mob.setDamage, mob, 40, xi.slot.MAIN), 'setDamage rejected a valid mob main update')
        assert(pcall(mob.setDamage, mob, 50, xi.slot.SUB), 'setDamage rejected a valid mob sub update')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        local initialPetDelay = pet:getBaseDelay()
        assert(pcall(pet.setDelay, pet, 180), 'pet setDelay rejected a valid no-op')
        assert(pet:getBaseDelay() == initialPetDelay, 'pet setDelay should not update state')
        assert(pcall(pet.setDamage, pet, 33, xi.slot.MAIN), 'pet setDamage rejected a valid update')
        player:despawnPet()

        assert(pcall(player.setDelay, player, 100), 'player setDelay rejected a valid no-op')
        assert(pcall(npc.setDelay, npc, 100), 'NPC setDelay rejected a valid no-op')
        assert(pcall(player.setDamage, player, 70, xi.slot.MAIN), 'player setDamage rejected a valid no-op')
        assert(pcall(npc.setDamage, npc, 70, xi.slot.MAIN), 'NPC setDamage rejected a valid no-op')
        assert(mob:getBaseDelay() == 240, 'invalid receiver setDelay should not change mob delay')

        assert(pcall(mob.setDamage, mob, 99, xi.slot.RANGED), 'setDamage rejected ranged slot')
        assert(pcall(mob.setDamage, mob, 99, xi.slot.AMMO), 'setDamage rejected invalid slot no-op')

        assert(not pcall(mob.setDelay), 'setDelay accepted missing self')
        assert(not pcall(mob.setDelay, mob), 'setDelay accepted missing delay')
        assert(not pcall(mob.setDelay, mob, 'bad'), 'setDelay accepted nonnumeric delay')
        assert(not pcall(player.setDelay, player, 'bad'), 'non-mob setDelay accepted nonnumeric delay')

        assert(not pcall(mob.setDamage), 'setDamage accepted missing self')
        assert(not pcall(mob.setDamage, mob), 'setDamage accepted missing damage')
        assert(not pcall(mob.setDamage, mob, 1), 'setDamage accepted missing slot')
        assert(not pcall(mob.setDamage, mob, 'bad', xi.slot.MAIN), 'setDamage accepted nonnumeric damage')
        assert(not pcall(mob.setDamage, mob, 1, 'bad'), 'setDamage accepted nonnumeric slot')
        assert(not pcall(player.setDamage, player, 'bad', xi.slot.MAIN), 'non-mob setDamage accepted nonnumeric damage')
        assert(not pcall(player.setDamage, player, 1, 'bad'), 'non-mob setDamage accepted nonnumeric slot')
    end)
end)
