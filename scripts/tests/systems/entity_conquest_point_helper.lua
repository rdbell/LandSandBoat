describe('Base entity conquest point helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates current-nation conquest points while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local initialNation = player:getNation()

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setNation(xi.nation.SANDORIA)
        assert(player:getCP() == 0, 'Initial San dOria conquest points were not zero')

        player:addCP(100)
        assert(player:getCP() == 100, 'addCP did not add conquest points')

        player:addCP(-25)
        assert(player:getCP() == 75, 'addCP with negative amount did not subtract conquest points')

        player:addCP(-100)
        assert(player:getCP() == 0, 'addCP allowed conquest points to become negative')

        player:addCP(200)
        player:delCP(50)
        assert(player:getCP() == 150, 'delCP did not subtract conquest points')

        player:delCP(1000)
        assert(player:getCP() == 0, 'delCP allowed conquest points to become negative')

        player:delCP(-30)
        assert(player:getCP() == 30, 'delCP with negative amount did not add conquest points')

        player:setNation(xi.nation.BASTOK)
        assert(player:getCP() == 0, 'Bastok conquest points shared San dOria balance')

        player:addCP(60)
        assert(player:getCP() == 60, 'Bastok addCP did not update current nation')

        player:setNation(xi.nation.SANDORIA)
        assert(player:getCP() == 30, 'San dOria balance was not preserved')

        npc:addCP(1000)
        mob:addCP(1000)
        npc:delCP(-1000)
        mob:delCP(-1000)
        assert(npc:getCP() == 0, 'NPC conquest point fallback changed')
        assert(mob:getCP() == 0, 'Mob conquest point fallback changed')

        player:setNation(initialNation)

        assert(not pcall(function()
            player:addCP('bad')
        end), 'addCP accepted a non-numeric value')

        assert(not pcall(function()
            player:delCP('bad')
        end), 'delCP accepted a non-numeric value')
    end)
end)
