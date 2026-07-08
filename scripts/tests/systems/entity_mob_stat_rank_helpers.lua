describe('Base entity mob stat-rank helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates mob stat ranks while non-mobs fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        local defaultSTRRank = mob:getStatRank(xi.stat.STR)
        mob:setStatRank(xi.stat.STR, 8)
        mob:setStatRank(0, 1)
        assert(mob:getStatRank(xi.stat.STR) == defaultSTRRank, 'invalid rank should preserve current STR rank')
        assert(mob:getStatRank(0) == 0, 'unsupported stat should return zero')

        mob:setStatRank(xi.stat.STR, 7)
        mob:setStatRank(xi.stat.ATT, 5)
        assert(mob:getStatRank(xi.stat.STR) == 7, 'expected STR rank to update')
        assert(mob:getStatRank(xi.stat.ATT) == 5, 'expected ATT rank to update')

        mob:setStatRank(xi.stat.STR, 8)
        mob:setStatRank(xi.stat.ATT, 6)
        assert(mob:getStatRank(xi.stat.STR) == 7, 'invalid STR rank should be ignored')
        assert(mob:getStatRank(xi.stat.ATT) == 5, 'invalid ATT rank should be ignored')

        player:setStatRank(xi.stat.STR, 1)
        npc:setStatRank(xi.stat.STR, 1)
        assert(player:getStatRank(xi.stat.STR) == 0, 'player stat rank should fall back to zero')
        assert(npc:getStatRank(xi.stat.STR) == 0, 'npc stat rank should fall back to zero')

        assert(not pcall(mob.getStatRank), 'getStatRank accepted missing self')
        assert(not pcall(mob.getStatRank, mob), 'getStatRank accepted missing stat')
        assert(not pcall(mob.getStatRank, mob, 'bad'), 'getStatRank accepted non-numeric stat')
        assert(not pcall(mob.setStatRank), 'setStatRank accepted missing self')
        assert(not pcall(mob.setStatRank, mob), 'setStatRank accepted missing stat')
        assert(not pcall(mob.setStatRank, mob, xi.stat.STR), 'setStatRank accepted missing rank')
        assert(not pcall(mob.setStatRank, mob, 'bad', 1), 'setStatRank accepted non-numeric stat')
        assert(not pcall(mob.setStatRank, mob, xi.stat.STR, 'bad'), 'setStatRank accepted non-numeric rank')
    end)
end)
