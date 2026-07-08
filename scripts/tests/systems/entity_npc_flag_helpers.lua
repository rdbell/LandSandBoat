describe('Base entity NPC flag helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('accepts NPC flag updates and ignores non-NPC receivers', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(pcall(npc.setNpcFlags, npc, 0x12345678), 'setNpcFlags rejected a valid NPC flag update')
        assert(pcall(npc.setNpcAlwaysRelevant, npc, true), 'setNpcAlwaysRelevant rejected a valid true update')
        assert(pcall(npc.setNpcAlwaysRelevant, npc, false), 'setNpcAlwaysRelevant rejected a valid false update')

        assert(pcall(player.setNpcFlags, player, 0xFFFFFFFF), 'setNpcFlags rejected a valid non-NPC no-op')
        assert(pcall(player.setNpcAlwaysRelevant, player, true), 'setNpcAlwaysRelevant rejected a valid non-NPC no-op')
        assert(pcall(mob.setNpcFlags, mob, 7), 'setNpcFlags rejected a valid mob no-op')
        assert(pcall(mob.setNpcAlwaysRelevant, mob, true), 'setNpcAlwaysRelevant rejected a valid mob no-op')

        assert(not pcall(npc.setNpcFlags), 'setNpcFlags accepted missing self')
        assert(not pcall(npc.setNpcFlags, npc), 'setNpcFlags accepted missing flags')
        assert(not pcall(npc.setNpcFlags, npc, 'bad'), 'setNpcFlags accepted non-numeric flags')
        assert(not pcall(npc.setNpcAlwaysRelevant), 'setNpcAlwaysRelevant accepted missing self')
        assert(not pcall(npc.setNpcAlwaysRelevant, npc), 'setNpcAlwaysRelevant accepted missing value')
        assert(not pcall(npc.setNpcAlwaysRelevant, npc, 'bad'), 'setNpcAlwaysRelevant accepted non-boolean value')
        assert(not pcall(player.setNpcFlags, player, 'bad'), 'non-NPC setNpcFlags accepted non-numeric flags')
        assert(not pcall(player.setNpcAlwaysRelevant, player, 'bad'), 'non-NPC setNpcAlwaysRelevant accepted non-boolean value')
    end)
end)
