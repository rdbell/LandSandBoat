describe('Base entity campaign allegiance helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player campaign allegiance while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        local initialAllegiance = player:getCampaignAllegiance()
        assert(type(initialAllegiance) == 'number', 'Player campaign allegiance was not numeric')

        assert(npc:getCampaignAllegiance() == 0, 'NPC campaign allegiance fallback was not zero')
        assert(mob:getCampaignAllegiance() == 0, 'Mob campaign allegiance fallback was not zero')

        player:setCampaignAllegiance(2)
        assert(player:getCampaignAllegiance() == 2, 'Player campaign allegiance did not become Bastok')

        player:setCampaignAllegiance(3)
        assert(player:getCampaignAllegiance() == 3, 'Player campaign allegiance did not become Windurst')

        npc:setCampaignAllegiance(3)
        mob:setCampaignAllegiance(3)
        assert(npc:getCampaignAllegiance() == 0, 'NPC campaign allegiance changed')
        assert(mob:getCampaignAllegiance() == 0, 'Mob campaign allegiance changed')

        player:setCampaignAllegiance(initialAllegiance)
        assert(player:getCampaignAllegiance() == initialAllegiance, 'Player campaign allegiance did not restore')

        assert(not pcall(function()
            player:setCampaignAllegiance('bad')
        end), 'setCampaignAllegiance accepted a non-numeric value')
    end)
end)
