describe('Base entity mob aggroable helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('reads and updates mob aggroable state while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(mob:isAggroable()) == 'boolean', 'expected mob aggroable state to be boolean')

        assert(pcall(mob.setIsAggroable, mob, true), 'setIsAggroable rejected a valid mob update')
        assert(mob:isAggroable() == true, 'expected mob aggroable state to update true')

        assert(pcall(mob.setIsAggroable, mob, false), 'setIsAggroable rejected a valid clear')
        assert(mob:isAggroable() == false, 'expected mob aggroable state to update false')

        assert(player:isAggroable() == false, 'expected player isAggroable fallback to false')
        assert(npc:isAggroable() == false, 'expected NPC isAggroable fallback to false')

        assert(pcall(player.setIsAggroable, player, true), 'player setIsAggroable rejected a valid no-op')
        assert(pcall(npc.setIsAggroable, npc, true), 'NPC setIsAggroable rejected a valid no-op')
        assert(player:isAggroable() == false, 'player setIsAggroable should not update state')
        assert(npc:isAggroable() == false, 'NPC setIsAggroable should not update state')
        assert(mob:isAggroable() == false, 'invalid receiver setIsAggroable should not change mob state')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(pet:isAggroable() == false, 'expected pet isAggroable fallback to false')
        assert(pcall(pet.setIsAggroable, pet, true), 'pet setIsAggroable rejected a valid no-op')
        assert(pet:isAggroable() == false, 'pet setIsAggroable should not update state')
        player:despawnPet()

        assert(not pcall(mob.isAggroable), 'isAggroable accepted missing self')
        assert(not pcall(mob.setIsAggroable), 'setIsAggroable accepted missing self')
        assert(not pcall(mob.setIsAggroable, mob), 'setIsAggroable accepted missing state')
        assert(not pcall(mob.setIsAggroable, mob, 'bad'), 'setIsAggroable accepted non-boolean state')
        assert(not pcall(player.setIsAggroable, player, 'bad'), 'non-mob setIsAggroable accepted non-boolean state')
    end)
end)
