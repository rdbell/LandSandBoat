describe('Base entity mob spell list helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('reads and updates mob spell lists while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(mob:getSpellListId()) == 'number', 'expected mob spell list ID to be numeric')
        assert(mob:hasSpellList() == false, 'expected Wild Rabbit to start without available spells')

        assert(pcall(mob.setSpellList, mob, 1), 'setSpellList rejected a valid mob update')
        assert(mob:getSpellListId() == 1, 'expected mob spell list ID to update')
        assert(mob:hasSpellList() == true, 'expected spell list 1 to add available spells')

        assert(pcall(mob.setSpellList, mob, 4), 'setSpellList rejected a valid high-level mob list')
        assert(mob:getSpellListId() == 4, 'expected high-level mob spell list ID to update')
        assert(mob:hasSpellList() == false, 'expected level-filtered spell list to have no available spells')

        assert(pcall(mob.setSpellList, mob, 0), 'setSpellList rejected the empty spell list')
        assert(mob:getSpellListId() == 0, 'expected mob spell list ID to clear')
        assert(mob:hasSpellList() == false, 'expected empty spell list to remove available spells')

        assert(player:getSpellListId() == 0, 'expected player getSpellListId fallback to zero')
        assert(npc:getSpellListId() == 0, 'expected NPC getSpellListId fallback to zero')
        assert(player:hasSpellList() == false, 'expected player hasSpellList fallback to false')
        assert(npc:hasSpellList() == false, 'expected NPC hasSpellList fallback to false')

        assert(pcall(player.setSpellList, player, 1), 'player setSpellList rejected a valid no-op')
        assert(pcall(npc.setSpellList, npc, 1), 'NPC setSpellList rejected a valid no-op')
        assert(player:getSpellListId() == 0, 'player setSpellList should not update spell list ID')
        assert(npc:getSpellListId() == 0, 'NPC setSpellList should not update spell list ID')
        assert(mob:getSpellListId() == 0, 'invalid receiver setSpellList should not change mob state')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(pcall(pet.setSpellList, pet, 1), 'pet setSpellList rejected a valid update')
        assert(pet:getSpellListId() == 1, 'expected pet spell list ID to update')
        assert(pet:hasSpellList() == true, 'expected pet spell list to add available spells')
        player:despawnPet()

        assert(not pcall(mob.getSpellListId), 'getSpellListId accepted missing self')
        assert(not pcall(mob.hasSpellList), 'hasSpellList accepted missing self')
        assert(not pcall(mob.setSpellList), 'setSpellList accepted missing self')
        assert(not pcall(mob.setSpellList, mob), 'setSpellList accepted missing spell list ID')
        assert(not pcall(mob.setSpellList, mob, 'bad'), 'setSpellList accepted non-numeric spell list ID')
        assert(not pcall(player.setSpellList, player, 'bad'), 'non-mob setSpellList accepted non-numeric spell list ID')
    end)
end)
