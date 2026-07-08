describe('Base entity mob behavior and link helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates mob behavior and link state while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(mob:getBehavior()) == 'number', 'expected behavior to be numeric')
        assert(type(mob:getLink()) == 'number', 'expected link state to be numeric')

        assert(pcall(mob.setBehavior, mob, 0x0408), 'setBehavior rejected a valid mob update')
        assert(mob:getBehavior() == 0x0408, 'expected mob behavior to update')

        assert(pcall(mob.setLink, mob, 1), 'setLink rejected a valid mob update')
        assert(mob:getLink() == 1, 'expected mob link state to update')

        assert(player:getBehavior() == 0, 'expected player getBehavior fallback to be zero')
        assert(npc:getBehavior() == 0, 'expected NPC getBehavior fallback to be zero')
        assert(player:getLink() == 0, 'expected player getLink fallback to be zero')
        assert(npc:getLink() == 0, 'expected NPC getLink fallback to be zero')

        assert(pcall(player.setBehavior, player, 0x0002), 'player setBehavior rejected a valid no-op')
        assert(pcall(npc.setBehavior, npc, 0x0004), 'NPC setBehavior rejected a valid no-op')
        assert(pcall(player.setLink, player, 2), 'player setLink rejected a valid no-op')
        assert(pcall(npc.setLink, npc, 3), 'NPC setLink rejected a valid no-op')
        assert(mob:getBehavior() == 0x0408, 'non-mob setBehavior should not change mob behavior')
        assert(mob:getLink() == 1, 'non-mob setLink should not change mob link state')

        assert(pcall(mob.setBehavior, mob, 0xFFFF), 'setBehavior rejected max uint16')
        assert(mob:getBehavior() == 0xFFFF, 'expected max uint16 behavior to update')

        assert(pcall(mob.setLink, mob, 0xFF), 'setLink rejected max uint8')
        assert(mob:getLink() == 0xFF, 'expected max uint8 link state to update')

        assert(not pcall(mob.getBehavior), 'getBehavior accepted missing self')
        assert(not pcall(mob.setBehavior), 'setBehavior accepted missing self')
        assert(not pcall(mob.setBehavior, mob), 'setBehavior accepted missing behavior')
        assert(not pcall(mob.setBehavior, mob, 'bad'), 'setBehavior accepted non-numeric behavior')
        assert(not pcall(player.setBehavior, player, 'bad'), 'non-mob setBehavior accepted non-numeric behavior')

        assert(not pcall(mob.getLink), 'getLink accepted missing self')
        assert(not pcall(mob.setLink), 'setLink accepted missing self')
        assert(not pcall(mob.setLink, mob), 'setLink accepted missing link value')
        assert(not pcall(mob.setLink, mob, 'bad'), 'setLink accepted non-numeric link value')
        assert(not pcall(player.setLink, player, 'bad'), 'non-mob setLink accepted non-numeric link value')
    end)
end)
