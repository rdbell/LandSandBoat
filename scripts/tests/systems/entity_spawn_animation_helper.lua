describe('Base entity spawn animation helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('accepts numeric spawn animation values for base entities', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setSpawnAnimation(0)
        npc:setSpawnAnimation(1)
        mob:setSpawnAnimation(2)

        assert(not pcall(function()
            npc:setSpawnAnimation('bad')
        end), 'setSpawnAnimation accepted a nonnumeric value')
    end)
end)
