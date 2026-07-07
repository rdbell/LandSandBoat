describe('Base entity appearance helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads player look fields and returns zero for non-player entities', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getRace() == xi.race.HUME_M, 'Player race was not HUME_M')
        assert(player:getFace() == 0, 'Player face was not Face1A')
        assert(player:getGender() == 1, 'Player gender was not male')
        assert(player:getSize() == 0, 'Player size was not small')

        assert(npc:getRace() == 0, 'NPC race fallback was not zero')
        assert(npc:getFace() == 0, 'NPC face fallback was not zero')
        assert(npc:getGender() == 0, 'NPC gender fallback was not zero')
        assert(npc:getSize() == 0, 'NPC size fallback was not zero')

        assert(mob:getRace() == 0, 'Mob race fallback was not zero')
        assert(mob:getFace() == 0, 'Mob face fallback was not zero')
        assert(mob:getGender() == 0, 'Mob gender fallback was not zero')
        assert(mob:getSize() == 0, 'Mob size fallback was not zero')
    end)
end)
