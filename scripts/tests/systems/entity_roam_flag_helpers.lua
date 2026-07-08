describe('Base entity roam flag helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates mob roam flags while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        local initialFlags = mob:getRoamFlags()
        assert(type(initialFlags) == 'number', 'expected roam flags to be numeric')

        assert(pcall(mob.setRoamFlags, mob, 0x0003), 'setRoamFlags rejected a valid mob update')
        assert(mob:getRoamFlags() == 0x0003, 'expected mob roam flags to update')

        assert(player:getRoamFlags() == 0, 'expected player getRoamFlags fallback to be zero')
        assert(npc:getRoamFlags() == 0, 'expected NPC getRoamFlags fallback to be zero')

        assert(pcall(player.setRoamFlags, player, 0x0005), 'player setRoamFlags rejected a valid no-op')
        assert(pcall(npc.setRoamFlags, npc, 0x0007), 'NPC setRoamFlags rejected a valid no-op')
        assert(mob:getRoamFlags() == 0x0003, 'non-mob setRoamFlags should not change mob roam flags')

        assert(pcall(mob.setRoamFlags, mob, 0xFFFF), 'setRoamFlags rejected max uint16')
        assert(mob:getRoamFlags() == 0xFFFF, 'expected max uint16 roam flags to update')

        assert(not pcall(mob.getRoamFlags), 'getRoamFlags accepted missing self')
        assert(not pcall(mob.setRoamFlags), 'setRoamFlags accepted missing self')
        assert(not pcall(mob.setRoamFlags, mob), 'setRoamFlags accepted missing flags')
        assert(not pcall(mob.setRoamFlags, mob, 'bad'), 'setRoamFlags accepted non-numeric flags')
        assert(not pcall(player.setRoamFlags, player, 'bad'), 'non-mob setRoamFlags accepted non-numeric flags')

        assert(pcall(mob.setRoamFlags, mob, initialFlags), 'failed to restore initial mob roam flags')
    end)
end)
