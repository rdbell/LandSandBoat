describe('Base entity mob mod helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('reads and updates mob mods while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(mob:getMobMod(xi.mobMod.MUG_GIL)) == 'number', 'expected mob mod to be numeric')
        assert(mob:getMobMod(xi.mobMod.GIL_MIN) == 0, 'expected unset mob mod to default to zero')

        assert(pcall(mob.setMobMod, mob, xi.mobMod.MUG_GIL, 123), 'setMobMod rejected a valid mob update')
        assert(mob:getMobMod(xi.mobMod.MUG_GIL) == 123, 'expected mob mod to update')

        assert(pcall(mob.addMobMod, mob, xi.mobMod.MUG_GIL, 5), 'addMobMod rejected a valid mob update')
        assert(mob:getMobMod(xi.mobMod.MUG_GIL) == 128, 'expected mob mod to add')

        assert(pcall(mob.delMobMod, mob, xi.mobMod.MUG_GIL, 8), 'delMobMod rejected a valid mob update')
        assert(mob:getMobMod(xi.mobMod.MUG_GIL) == 120, 'expected mob mod to subtract')

        assert(pcall(mob.setMobMod, mob, xi.mobMod.GIL_MIN, -321), 'setMobMod rejected a negative value')
        assert(mob:getMobMod(xi.mobMod.GIL_MIN) == -321, 'expected negative mob mod to update')

        assert(pcall(mob.setMobMod, mob, xi.mobMod.NO_DROPS, 32767), 'setMobMod rejected max int16')
        assert(mob:getMobMod(xi.mobMod.NO_DROPS) == 32767, 'expected max int16 mob mod to update')

        assert(pcall(mob.setMobMod, mob, xi.mobMod.NO_DROPS, -32768), 'setMobMod rejected min int16')
        assert(mob:getMobMod(xi.mobMod.NO_DROPS) == -32768, 'expected min int16 mob mod to update')

        assert(player:getMobMod(xi.mobMod.MUG_GIL) == 0, 'expected player getMobMod fallback to be zero')
        assert(npc:getMobMod(xi.mobMod.MUG_GIL) == 0, 'expected NPC getMobMod fallback to be zero')

        assert(pcall(player.setMobMod, player, xi.mobMod.MUG_GIL, 11), 'player setMobMod rejected a valid no-op')
        assert(pcall(player.addMobMod, player, xi.mobMod.MUG_GIL, 12), 'player addMobMod rejected a valid no-op')
        assert(pcall(player.delMobMod, player, xi.mobMod.MUG_GIL, 13), 'player delMobMod rejected a valid no-op')
        assert(pcall(npc.setMobMod, npc, xi.mobMod.MUG_GIL, 21), 'NPC setMobMod rejected a valid no-op')
        assert(pcall(npc.addMobMod, npc, xi.mobMod.MUG_GIL, 22), 'NPC addMobMod rejected a valid no-op')
        assert(pcall(npc.delMobMod, npc, xi.mobMod.MUG_GIL, 23), 'NPC delMobMod rejected a valid no-op')
        assert(mob:getMobMod(xi.mobMod.MUG_GIL) == 120, 'invalid receiver mob mod helpers should not change mob state')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(pcall(pet.setMobMod, pet, xi.mobMod.MUG_GIL, 4), 'pet setMobMod rejected a valid update')
        assert(pcall(pet.addMobMod, pet, xi.mobMod.MUG_GIL, 6), 'pet addMobMod rejected a valid update')
        assert(pcall(pet.delMobMod, pet, xi.mobMod.MUG_GIL, 3), 'pet delMobMod rejected a valid update')
        assert(pet:getMobMod(xi.mobMod.MUG_GIL) == 7, 'expected pet mob mod to update')
        player:despawnPet()

        assert(not pcall(mob.getMobMod), 'getMobMod accepted missing self')
        assert(not pcall(mob.getMobMod, mob), 'getMobMod accepted missing mod ID')
        assert(not pcall(mob.getMobMod, mob, 'bad'), 'getMobMod accepted non-numeric mod ID')

        assert(not pcall(mob.setMobMod), 'setMobMod accepted missing self')
        assert(not pcall(mob.setMobMod, mob), 'setMobMod accepted missing mod ID')
        assert(not pcall(mob.setMobMod, mob, xi.mobMod.MUG_GIL), 'setMobMod accepted missing value')
        assert(not pcall(mob.setMobMod, mob, 'bad', 1), 'setMobMod accepted non-numeric mod ID')
        assert(not pcall(mob.setMobMod, mob, xi.mobMod.MUG_GIL, 'bad'), 'setMobMod accepted non-numeric value')
        assert(not pcall(player.setMobMod, player, xi.mobMod.MUG_GIL, 'bad'), 'non-mob setMobMod accepted non-numeric value')

        assert(not pcall(mob.addMobMod), 'addMobMod accepted missing self')
        assert(not pcall(mob.addMobMod, mob), 'addMobMod accepted missing mod ID')
        assert(not pcall(mob.addMobMod, mob, xi.mobMod.MUG_GIL), 'addMobMod accepted missing value')
        assert(not pcall(mob.addMobMod, mob, 'bad', 1), 'addMobMod accepted non-numeric mod ID')
        assert(not pcall(mob.addMobMod, mob, xi.mobMod.MUG_GIL, 'bad'), 'addMobMod accepted non-numeric value')

        assert(not pcall(mob.delMobMod), 'delMobMod accepted missing self')
        assert(not pcall(mob.delMobMod, mob), 'delMobMod accepted missing mod ID')
        assert(not pcall(mob.delMobMod, mob, xi.mobMod.MUG_GIL), 'delMobMod accepted missing value')
        assert(not pcall(mob.delMobMod, mob, 'bad', 1), 'delMobMod accepted non-numeric mod ID')
        assert(not pcall(mob.delMobMod, mob, xi.mobMod.MUG_GIL, 'bad'), 'delMobMod accepted non-numeric value')
    end)
end)
