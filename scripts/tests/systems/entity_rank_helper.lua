describe('Base entity rank helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
            nation = xi.nation.SANDORIA,
        })
    end)

    it('reads and updates player rank and rank points while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getRank(xi.nation.SANDORIA) == 1, 'Initial San dOria rank was not one')
        assert(player:getRank(xi.nation.BASTOK) == 1, 'Initial Bastok rank was not one')
        assert(player:getRankPoints() == 0, 'Initial rank points were not zero')

        player:setRank(5)
        assert(player:getRank(xi.nation.SANDORIA) == 5, 'setRank did not update current nation rank')
        assert(player:getRank(xi.nation.BASTOK) == 1, 'setRank changed another nation rank')

        player:setRank(257)
        assert(player:getRank(xi.nation.SANDORIA) == 1, 'setRank did not wrap uint8 input')

        player:addRankPoints(3990)
        assert(player:getRankPoints() == 3990, 'addRankPoints did not accumulate rank points')

        player:addRankPoints(20)
        assert(player:getRankPoints() == 4000, 'addRankPoints did not cap rank points at 4000')

        player:addRankPoints(65535)
        assert(player:getRankPoints() == 3999, 'addRankPoints did not preserve uint16 addition behavior')

        player:setRankPoints(65535)
        assert(player:getRankPoints() == 4000, 'setRankPoints did not cap uint16 input')

        player:setRankPoints(65537)
        assert(player:getRankPoints() == 1, 'setRankPoints did not wrap uint16 input')

        npc:setRank(5)
        npc:addRankPoints(100)
        npc:setRankPoints(100)
        mob:setRank(5)
        mob:addRankPoints(100)
        mob:setRankPoints(100)
        assert(npc:getRank(xi.nation.SANDORIA) == 0, 'NPC rank fallback changed after setters')
        assert(mob:getRank(xi.nation.SANDORIA) == 0, 'Mob rank fallback changed after setters')
        assert(npc:getRankPoints() == 0, 'NPC rank point fallback changed after setters')
        assert(mob:getRankPoints() == 0, 'Mob rank point fallback changed after setters')
    end)
end)
