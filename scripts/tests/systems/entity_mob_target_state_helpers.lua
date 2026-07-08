describe('Base entity mob target state helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('updates mob and NPC target flags while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(pcall(mob.setAggressive, mob, true), 'setAggressive rejected a valid mob update')
        assert(pcall(mob.setAggressive, mob, false), 'setAggressive rejected a valid mob clear')
        assert(pcall(mob.setTrueDetection, mob, true), 'setTrueDetection rejected a valid mob update')
        assert(pcall(mob.setTrueDetection, mob, false), 'setTrueDetection rejected a valid mob clear')

        assert(pcall(player.setAggressive, player, true), 'player setAggressive rejected a valid no-op')
        assert(pcall(npc.setAggressive, npc, true), 'NPC setAggressive rejected a valid no-op')
        assert(pcall(player.setTrueDetection, player, true), 'player setTrueDetection rejected a valid no-op')
        assert(pcall(npc.setTrueDetection, npc, true), 'NPC setTrueDetection rejected a valid no-op')

        assert(pcall(player.setUnkillable, player, true), 'player setUnkillable rejected a valid update')
        assert(pcall(player.setUnkillable, player, false), 'player setUnkillable rejected a valid clear')
        assert(pcall(mob.setUnkillable, mob, true), 'mob setUnkillable rejected a valid update')
        assert(pcall(mob.setUnkillable, mob, false), 'mob setUnkillable rejected a valid clear')
        assert(pcall(npc.setUnkillable, npc, true), 'NPC setUnkillable rejected a valid no-op')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(pcall(pet.setAggressive, pet, true), 'pet setAggressive rejected a valid no-op')
        assert(pcall(pet.setTrueDetection, pet, true), 'pet setTrueDetection rejected a valid no-op')
        assert(pcall(pet.setUnkillable, pet, true), 'pet setUnkillable rejected a valid update')
        assert(pcall(pet.setUnkillable, pet, false), 'pet setUnkillable rejected a valid clear')
        assert(pet:getUntargetable() == false, 'expected pet getUntargetable fallback to false')
        assert(pcall(pet.setUntargetable, pet, true), 'pet setUntargetable rejected a valid no-op')
        assert(pet:getUntargetable() == false, 'pet setUntargetable should not update state')
        player:despawnPet()

        assert(mob:getUntargetable() == false, 'expected mob untargetable to default false')
        assert(npc:getUntargetable() == false, 'expected NPC untargetable to default false')
        assert(player:getUntargetable() == false, 'expected player getUntargetable fallback false')

        assert(pcall(mob.setUntargetable, mob, true), 'setUntargetable rejected a valid mob update')
        assert(mob:getUntargetable() == true, 'expected mob untargetable to update true')
        assert(pcall(mob.setUntargetable, mob, false), 'setUntargetable rejected a valid mob clear')
        assert(mob:getUntargetable() == false, 'expected mob untargetable to clear')

        assert(pcall(npc.setUntargetable, npc, true), 'setUntargetable rejected a valid NPC update')
        assert(npc:getUntargetable() == true, 'expected NPC untargetable to update true')
        assert(pcall(npc.setUntargetable, npc, false), 'setUntargetable rejected a valid NPC clear')
        assert(npc:getUntargetable() == false, 'expected NPC untargetable to clear')

        assert(pcall(player.setUntargetable, player, true), 'player setUntargetable rejected a valid no-op')
        assert(player:getUntargetable() == false, 'player setUntargetable should not update state')

        assert(not pcall(mob.setAggressive), 'setAggressive accepted missing self')
        assert(not pcall(mob.setAggressive, mob), 'setAggressive accepted missing state')
        assert(not pcall(mob.setAggressive, mob, 'bad'), 'setAggressive accepted non-boolean state')
        assert(not pcall(player.setAggressive, player, 'bad'), 'non-mob setAggressive accepted non-boolean state')

        assert(not pcall(mob.setTrueDetection), 'setTrueDetection accepted missing self')
        assert(not pcall(mob.setTrueDetection, mob), 'setTrueDetection accepted missing state')
        assert(not pcall(mob.setTrueDetection, mob, 'bad'), 'setTrueDetection accepted non-boolean state')
        assert(not pcall(player.setTrueDetection, player, 'bad'), 'non-mob setTrueDetection accepted non-boolean state')

        assert(not pcall(mob.setUnkillable), 'setUnkillable accepted missing self')
        assert(not pcall(mob.setUnkillable, mob), 'setUnkillable accepted missing state')
        assert(not pcall(mob.setUnkillable, mob, 'bad'), 'setUnkillable accepted non-boolean state')
        assert(not pcall(npc.setUnkillable, npc, 'bad'), 'non-battle setUnkillable accepted non-boolean state')

        assert(not pcall(mob.setUntargetable), 'setUntargetable accepted missing self')
        assert(not pcall(mob.setUntargetable, mob), 'setUntargetable accepted missing state')
        assert(not pcall(mob.setUntargetable, mob, 'bad'), 'setUntargetable accepted non-boolean state')
        assert(not pcall(player.setUntargetable, player, 'bad'), 'invalid setUntargetable receiver accepted non-boolean state')
        assert(not pcall(mob.getUntargetable), 'getUntargetable accepted missing self')
    end)
end)
