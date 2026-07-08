describe('Base entity mob flag helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates mob flags while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        local initialFlags = mob:getMobFlags()
        assert(type(initialFlags) == 'number', 'expected mob flags to be numeric')

        assert(pcall(mob.setMobFlags, mob, 0x00000483), 'mob setMobFlags rejected a valid self update')
        assert(mob:getMobFlags() == 0x00000483, 'expected mob self update to change mob flags')

        assert(pcall(player.setMobFlags, player, 0x0000048B, mob:getID()), 'player setMobFlags rejected a valid explicit mob ID update')
        assert(mob:getMobFlags() == 0x0000048B, 'expected player explicit mob ID update to change mob flags')

        assert(pcall(player.setMobFlags, player, 0x00000491, 0xFFFFFFFF), 'player setMobFlags rejected a missing explicit mob ID no-op')
        assert(mob:getMobFlags() == 0x0000048B, 'missing explicit mob ID should not change mob flags')

        assert(player:getMobFlags() == 0, 'expected player getMobFlags fallback to be zero')
        assert(npc:getMobFlags() == 0, 'expected NPC getMobFlags fallback to be zero')
        assert(pcall(npc.setMobFlags, npc, 0x00000493), 'NPC setMobFlags rejected a valid no-op')
        assert(mob:getMobFlags() == 0x0000048B, 'NPC setMobFlags should not change mob flags')

        assert(not pcall(mob.getMobFlags), 'getMobFlags accepted missing self')
        assert(not pcall(mob.setMobFlags), 'setMobFlags accepted missing self')
        assert(not pcall(mob.setMobFlags, mob), 'setMobFlags accepted missing flags')
        assert(not pcall(mob.setMobFlags, mob, 'bad'), 'setMobFlags accepted non-numeric flags')
        assert(not pcall(player.setMobFlags, player, 1, 'bad'), 'setMobFlags accepted non-numeric mob ID')
    end)
end)
