describe('Base entity stolen item flag helper bindings', function()
    it('accepts mob stolen/despoiled flag updates while invalid receivers no-op', function()
        local player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(pcall(mob.itemStolen, mob, true), 'itemStolen rejected true for a mob')
        assert(pcall(mob.itemStolen, mob, false), 'itemStolen rejected false for a mob')
        assert(pcall(mob.itemDespoiled, mob, true), 'itemDespoiled rejected true for a mob')
        assert(pcall(mob.itemDespoiled, mob, false), 'itemDespoiled rejected false for a mob')

        assert(pcall(npc.itemStolen, npc, false), 'NPC itemStolen should accept a valid no-op')
        assert(pcall(npc.itemDespoiled, npc, false), 'NPC itemDespoiled should accept a valid no-op')
        assert(pcall(player.itemStolen, player, false), 'player itemStolen should accept a valid no-op')
        assert(pcall(player.itemDespoiled, player, false), 'player itemDespoiled should accept a valid no-op')

        assert(not pcall(mob.itemStolen), 'itemStolen accepted missing self')
        assert(not pcall(mob.itemStolen, mob), 'itemStolen accepted missing value')
        assert(not pcall(mob.itemStolen, mob, 'bad'), 'itemStolen accepted non-boolean value')
        assert(not pcall(player.itemStolen, player, 'bad'), 'invalid itemStolen receiver accepted non-boolean value')
        assert(not pcall(mob.itemDespoiled), 'itemDespoiled accepted missing self')
        assert(not pcall(mob.itemDespoiled, mob), 'itemDespoiled accepted missing value')
        assert(not pcall(mob.itemDespoiled, mob, 'bad'), 'itemDespoiled accepted non-boolean value')
        assert(not pcall(player.itemDespoiled, player, 'bad'), 'invalid itemDespoiled receiver accepted non-boolean value')
    end)
end)
