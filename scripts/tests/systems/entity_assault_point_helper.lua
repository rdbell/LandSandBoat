describe('Base entity assault point helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player assault point balances while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        for region = 0, 4 do
            assert(player:getAssaultPoint(region) == 0, 'Initial assault point balance was not zero')
        end

        player:addAssaultPoint(0, 100)
        assert(player:getAssaultPoint(0) == 100, 'addAssaultPoint did not add assault points')

        player:addAssaultPoint(0, -25)
        assert(player:getAssaultPoint(0) == 75, 'addAssaultPoint with negative amount did not subtract points')

        player:addAssaultPoint(0, -100)
        assert(player:getAssaultPoint(0) == 0, 'addAssaultPoint allowed points to become negative')

        player:addAssaultPoint(0, 200)
        player:delAssaultPoint(0, 50)
        assert(player:getAssaultPoint(0) == 150, 'delAssaultPoint did not subtract points')

        player:delAssaultPoint(0, 1000)
        assert(player:getAssaultPoint(0) == 0, 'delAssaultPoint allowed points to become negative')

        player:delAssaultPoint(0, -30)
        assert(player:getAssaultPoint(0) == 30, 'delAssaultPoint with negative amount did not add points')

        player:addAssaultPoint(1, 11)
        player:addAssaultPoint(2, 22)
        player:addAssaultPoint(3, 33)
        player:addAssaultPoint(4, 44)
        assert(player:getAssaultPoint(1) == 11, 'Mamool Ja Training Grounds balance was not independent')
        assert(player:getAssaultPoint(2) == 22, 'Lebros Cavern balance was not independent')
        assert(player:getAssaultPoint(3) == 33, 'Periqia balance was not independent')
        assert(player:getAssaultPoint(4) == 44, 'Ilrusi Atoll balance was not independent')

        player:addAssaultPoint(5, 999)
        player:delAssaultPoint(5, -999)
        assert(player:getAssaultPoint(5) == 0, 'Invalid assault region became readable')
        assert(player:getAssaultPoint(0) == 30, 'Invalid assault region changed Leujaoam points')

        npc:addAssaultPoint(0, 1000)
        mob:addAssaultPoint(0, 1000)
        npc:delAssaultPoint(0, -1000)
        mob:delAssaultPoint(0, -1000)
        assert(npc:getAssaultPoint(0) == 0, 'NPC assault point fallback changed')
        assert(mob:getAssaultPoint(0) == 0, 'Mob assault point fallback changed')

        assert(not pcall(function()
            player:getAssaultPoint('bad')
        end), 'getAssaultPoint accepted a non-numeric region')

        assert(not pcall(function()
            player:addAssaultPoint('bad', 1)
        end), 'addAssaultPoint accepted a non-numeric region')

        assert(not pcall(function()
            player:addAssaultPoint(0, 'bad')
        end), 'addAssaultPoint accepted a non-numeric amount')

        assert(not pcall(function()
            player:delAssaultPoint('bad', 1)
        end), 'delAssaultPoint accepted a non-numeric region')

        assert(not pcall(function()
            player:delAssaultPoint(0, 'bad')
        end), 'delAssaultPoint accepted a non-numeric amount')
    end)
end)
