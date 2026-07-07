describe('Base entity animation sub helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates entity animation sub values', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getAnimationSub() == 0, 'Player initial animation sub was not zero')
        assert(npc:getAnimationSub() == 0, 'NPC initial animation sub was not zero')
        assert(mob:getAnimationSub() == 0, 'Mob initial animation sub was not zero')

        player:setAnimationSub(1)
        npc:setAnimationSub(8, false)
        mob:setAnimationSub(33, true)

        assert(player:getAnimationSub() == 1, 'Player animation sub was not updated')
        assert(npc:getAnimationSub() == 8, 'NPC animation sub was not updated')
        assert(mob:getAnimationSub() == 33, 'Mob animation sub was not updated')

        npc:setAnimationSub(9)
        assert(npc:getAnimationSub() == 9, 'NPC animation sub was not updated a second time')

        player:setAnimationSub(0)
        assert(player:getAnimationSub() == 0, 'Player animation sub was not reset')
    end)
end)
