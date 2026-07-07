describe('Base entity nation helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player nation while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        local initialNation = player:getNation()
        assert(type(initialNation) == 'number', 'Player nation was not numeric')

        assert(npc:getNation() == 0, 'NPC nation fallback was not zero')
        assert(mob:getNation() == 0, 'Mob nation fallback was not zero')

        player:setNation(xi.nation.BASTOK)
        assert(player:getNation() == xi.nation.BASTOK, 'Player nation did not become Bastok')

        player:setNation(xi.nation.WINDURST)
        assert(player:getNation() == xi.nation.WINDURST, 'Player nation did not become Windurst')

        npc:setNation(xi.nation.WINDURST)
        mob:setNation(xi.nation.WINDURST)
        assert(npc:getNation() == 0, 'NPC nation changed')
        assert(mob:getNation() == 0, 'Mob nation changed')

        player:setNation(initialNation)
        assert(player:getNation() == initialNation, 'Player nation did not restore')

        assert(not pcall(function()
            player:setNation('bad')
        end), 'setNation accepted a non-numeric value')
    end)
end)
