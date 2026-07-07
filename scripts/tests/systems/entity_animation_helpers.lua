describe('Base entity animation helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates entity animations', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getAnimation() == 0, 'Player initial animation was not zero')
        assert(npc:getAnimation() == 0, 'NPC initial animation was not zero')
        assert(mob:getAnimation() == 0, 'Mob initial animation was not zero')

        player:setAnimation(1)
        npc:setAnimation(8)
        mob:setAnimation(33)

        assert(player:getAnimation() == 1, 'Player animation was not updated')
        assert(npc:getAnimation() == 8, 'NPC animation was not updated')
        assert(mob:getAnimation() == 33, 'Mob animation was not updated')

        npc:setAnimation(9)
        assert(npc:getAnimation() == 9, 'NPC animation was not updated a second time')

        player:setAnimation(0)
        assert(player:getAnimation() == 0, 'Player animation was not reset')
    end)
end)
