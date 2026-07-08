describe('Base entity gil helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player gil while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setGil(1000)
        assert(player:getGil() == 1000, 'setGil did not set gil')

        player:addGil(250)
        assert(player:getGil() == 1250, 'addGil did not add gil')

        player:addGil(-200)
        assert(player:getGil() == 1050, 'addGil with negative amount did not subtract gil')

        player:addGil(-2000)
        assert(player:getGil() == 1050, 'addGil allowed gil to become negative')

        assert(player:delGil(50) == true, 'delGil did not report success')
        assert(player:getGil() == 1000, 'delGil did not subtract gil')

        assert(player:delGil(0) == true, 'delGil(0) did not report success')
        assert(player:getGil() == 1000, 'delGil(0) changed gil')

        assert(player:delGil(1001) == false, 'delGil reported success without enough gil')
        assert(player:getGil() == 1000, 'Failed delGil changed gil')

        assert(player:delGil(-1) == false, 'delGil accepted negative gil')
        assert(player:getGil() == 1000, 'Negative delGil changed gil')

        player:setGil(400)
        assert(player:getGil() == 400, 'setGil did not lower gil')

        player:setGil(0)
        assert(player:getGil() == 0, 'setGil did not clear gil')

        npc:setGil(1000)
        mob:setGil(1000)
        npc:addGil(1000)
        mob:addGil(1000)
        assert(npc:delGil(1) == false, 'NPC delGil reported success')
        assert(mob:delGil(1) == false, 'Mob delGil reported success')
        assert(npc:getGil() == 0, 'NPC gil fallback changed')
        assert(mob:getGil() == 0, 'Mob gil fallback changed')

        player:setGil(65537)
        assert(player:getGil() == 65537, 'setGil did not preserve int32 amount')

        player:setGil(1000000000)
        assert(player:getGil() == 999999999, 'setGil did not clamp to gil stack size')

        player:addGil(10)
        assert(player:getGil() == 999999999, 'addGil above gil stack size changed capped gil')

        player:setGil(0)
    end)
end)
