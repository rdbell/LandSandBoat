describe('Base entity model size helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates battle entity model sizes while NPCs fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:setModelSize(2)
        player:setModelSize(1)
        npc:setModelSize(2)

        assert(mob:getModelSize() == 2, 'expected mob model size to update')
        assert(player:getModelSize() == 1, 'expected player model size to update')
        assert(npc:getModelSize() == 0, 'expected NPC model size to fall back to zero')

        mob:setModelSize(9)
        player:setModelSize(4)

        assert(mob:getModelSize() == 3, 'expected mob model size to clamp to 3')
        assert(player:getModelSize() == 3, 'expected player model size to clamp to 3')

        mob:setModelSize(0)
        assert(mob:getModelSize() == 0, 'expected model size zero to be accepted')

        assert(not pcall(mob.getModelSize), 'getModelSize accepted missing self')
        assert(not pcall(mob.setModelSize), 'setModelSize accepted missing self')
        assert(not pcall(mob.setModelSize, mob), 'setModelSize accepted missing size')
        assert(not pcall(mob.setModelSize, mob, 'bad'), 'setModelSize accepted non-numeric size')
        assert(not pcall(npc.setModelSize, npc, 'bad'), 'NPC setModelSize accepted non-numeric size')
    end)
end)
