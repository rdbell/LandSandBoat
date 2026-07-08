describe('Base entity hitbox size helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates battle entity hitbox sizes while NPCs fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:setHitboxSize(2.5)
        player:setHitboxSize(1.25)
        npc:setHitboxSize(4.5)

        assert(mob:getHitboxSize() == 2.5, 'expected mob hitbox size to update')
        assert(player:getHitboxSize() == 1.25, 'expected player hitbox size to update')
        assert(npc:getHitboxSize() == 0, 'expected NPC hitbox size to fall back to zero')

        mob:setHitboxSize(-2.25)
        player:setHitboxSize(0)

        assert(mob:getHitboxSize() == -2.25, 'expected negative hitbox size to be stored')
        assert(player:getHitboxSize() == 0, 'expected zero hitbox size to be stored')

        assert(not pcall(mob.getHitboxSize), 'getHitboxSize accepted missing self')
        assert(not pcall(mob.setHitboxSize), 'setHitboxSize accepted missing self')
        assert(not pcall(mob.setHitboxSize, mob), 'setHitboxSize accepted missing size')
        assert(not pcall(mob.setHitboxSize, mob, 'bad'), 'setHitboxSize accepted non-numeric size')
        assert(not pcall(npc.setHitboxSize, npc, 'bad'), 'NPC setHitboxSize accepted non-numeric size')
    end)
end)
