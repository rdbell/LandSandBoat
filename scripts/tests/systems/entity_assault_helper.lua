describe('Base entity assault helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player assault state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getCurrentAssault() == 0, 'Initial current assault was not zero')
        assert(player:hasCompletedAssault(12) == false, 'Initial assault unexpectedly completed')

        player:addAssault(12)
        assert(player:getCurrentAssault() == 12, 'addAssault did not set current assault')
        assert(player:hasCompletedAssault(12) == false, 'addAssault unexpectedly completed assault')

        player:addAssault(13)
        assert(player:getCurrentAssault() == 13, 'Repeated addAssault did not replace current assault')
        assert(player:hasCompletedAssault(12) == false, 'Replacing current assault completed previous assault')

        player:delAssault(12)
        assert(player:getCurrentAssault() == 13, 'delAssault for non-current assault changed current')

        player:delAssault(13)
        assert(player:getCurrentAssault() == 0, 'delAssault did not clear current assault')
        assert(player:hasCompletedAssault(13) == false, 'delAssault completed current assault')

        player:addAssault(14)
        player:completeAssault(14)
        assert(player:getCurrentAssault() == 0, 'completeAssault did not clear current assault')
        assert(player:hasCompletedAssault(14) == true, 'completeAssault did not set completed assault')

        player:addAssault(15)
        player:completeAssault(16)
        assert(player:getCurrentAssault() == 0, 'completeAssault for non-current assault did not clear current')
        assert(player:hasCompletedAssault(15) == false, 'completeAssault for another assault completed current assault')
        assert(player:hasCompletedAssault(16) == true, 'completeAssault for non-current assault did not complete requested assault')

        player:addAssault(127)
        player:completeAssault(127)
        assert(player:hasCompletedAssault(127) == true, 'Max valid assault ID did not complete')

        player:addAssault(257)
        assert(player:getCurrentAssault() == 1, 'Assault ID did not wrap through uint8 input')
        player:completeAssault(257)
        assert(player:hasCompletedAssault(1) == true, 'Wrapped assault completion did not set bit 1')

        npc:addAssault(12)
        mob:addAssault(12)
        npc:completeAssault(12)
        mob:completeAssault(12)
        npc:delAssault(12)
        mob:delAssault(12)
        assert(npc:getCurrentAssault() == 0, 'NPC current assault fallback changed after setters')
        assert(mob:getCurrentAssault() == 0, 'Mob current assault fallback changed after setters')
        assert(npc:hasCompletedAssault(12) == false, 'NPC completed assault fallback changed')
        assert(mob:hasCompletedAssault(12) == false, 'Mob completed assault fallback changed')
    end)
end)
