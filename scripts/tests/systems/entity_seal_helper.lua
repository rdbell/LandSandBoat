describe('Base entity seal helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player seal balances while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        for sealType = 0, 4 do
            assert(player:getSeals(sealType) == 0, 'Initial seal balance was not zero')
        end

        player:addSeals(100, 0)
        assert(player:getSeals(0) == 100, 'addSeals did not add beastman seals')

        player:addSeals(-25, 0)
        assert(player:getSeals(0) == 75, 'addSeals with negative amount did not subtract seals')

        player:addSeals(-100, 0)
        assert(player:getSeals(0) == 0, 'addSeals allowed seals to become negative')

        player:addSeals(200, 0)
        player:delSeals(50, 0)
        assert(player:getSeals(0) == 150, 'delSeals did not subtract seals')

        player:delSeals(1000, 0)
        assert(player:getSeals(0) == 0, 'delSeals allowed seals to become negative')

        player:delSeals(-30, 0)
        assert(player:getSeals(0) == 30, 'delSeals with negative amount did not add seals')

        player:addSeals(11, 1)
        player:addSeals(22, 2)
        player:addSeals(33, 3)
        player:addSeals(44, 4)
        assert(player:getSeals(1) == 11, 'Kindred seal balance was not independent')
        assert(player:getSeals(2) == 22, 'Kindred crest balance was not independent')
        assert(player:getSeals(3) == 33, 'High kindred crest balance was not independent')
        assert(player:getSeals(4) == 44, 'Sacred kindred crest balance was not independent')

        player:addSeals(999, 5)
        player:delSeals(-999, 5)
        assert(player:getSeals(5) == 0, 'Invalid seal type became readable')
        assert(player:getSeals(0) == 30, 'Invalid seal type changed beastman seals')

        npc:addSeals(1000, 0)
        mob:addSeals(1000, 0)
        npc:delSeals(-1000, 0)
        mob:delSeals(-1000, 0)
        assert(npc:getSeals(0) == 0, 'NPC seal fallback changed')
        assert(mob:getSeals(0) == 0, 'Mob seal fallback changed')

        assert(not pcall(function()
            player:getSeals('bad')
        end), 'getSeals accepted a non-numeric type')

        assert(not pcall(function()
            player:addSeals('bad', 0)
        end), 'addSeals accepted a non-numeric amount')

        assert(not pcall(function()
            player:addSeals(1, 'bad')
        end), 'addSeals accepted a non-numeric type')

        assert(not pcall(function()
            player:delSeals('bad', 0)
        end), 'delSeals accepted a non-numeric amount')

        assert(not pcall(function()
            player:delSeals(1, 'bad')
        end), 'delSeals accepted a non-numeric type')
    end)
end)
