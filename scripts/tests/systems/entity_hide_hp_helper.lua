describe('Base entity hide HP helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('toggles mob HP visibility and safely ignores invalid entities', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:setMobFlags(0x200)
        mob:hideHP(true)
        assert(mob:getMobFlags() == 0x300, 'hideHP(true) should set FLAG_HIDE_HP without clearing other flags')
        mob:hideHP(false)
        assert(mob:getMobFlags() == 0x200, 'hideHP(false) should preserve other flags')

        npc:setNpcFlags(0x200)
        assert(pcall(npc.hideHP, npc, true), 'NPC hideHP should accept a boolean')
        assert(pcall(npc.hideHP, npc, false), 'NPC hideHP should accept false')
        assert(pcall(player.hideHP, player, true), 'player hideHP should be a safe no-op')

        assert(not pcall(mob.hideHP), 'hideHP accepted missing self')
        assert(not pcall(mob.hideHP, mob), 'hideHP accepted missing value')
        assert(not pcall(mob.hideHP, mob, 'bad'), 'hideHP accepted non-boolean value')
    end)
end)
