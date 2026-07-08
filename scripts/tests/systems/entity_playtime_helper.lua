describe('Base entity playtime helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads player playtime and creation time while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        local initial = player:getPlaytime(false)
        assert(initial >= 60, 'Player initial playtime did not include seeded test playtime')
        assert(player:getPlaytime(false) == initial, 'getPlaytime(false) updated playtime')

        xi.test.world:tick()

        assert(player:getPlaytime(false) == initial, 'getPlaytime(false) changed after time advanced')

        local updated = player:getPlaytime(true)
        assert(updated >= initial, 'getPlaytime(true) moved playtime backwards')
        assert(player:getPlaytime(false) == updated, 'getPlaytime(true) did not save updated playtime')
        assert(player:getPlaytime() >= updated, 'getPlaytime() default did not update playtime')

        assert(player:getTimeCreated() > 0, 'Player creation timestamp was not populated')

        assert(npc:getPlaytime() == 0, 'NPC playtime fallback was not zero')
        assert(mob:getPlaytime(false) == 0, 'Mob playtime fallback was not zero')
        assert(npc:getTimeCreated() == 0, 'NPC creation timestamp fallback was not zero')
        assert(mob:getTimeCreated() == 0, 'Mob creation timestamp fallback was not zero')
    end)
end)
