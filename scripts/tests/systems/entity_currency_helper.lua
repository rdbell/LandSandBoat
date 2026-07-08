describe('Base entity currency helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player currency balances while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getCurrency('zeni_point') == 0, 'Initial zeni balance was not zero')

        player:setCurrency('zeni_point', 100)
        assert(player:getCurrency('zeni_point') == 100, 'setCurrency did not set zeni')

        player:addCurrency('zeni_point', 25)
        assert(player:getCurrency('zeni_point') == 125, 'addCurrency did not add zeni')

        player:addCurrency('zeni_point', -50)
        assert(player:getCurrency('zeni_point') == 75, 'addCurrency with negative amount did not subtract zeni')

        player:addCurrency('zeni_point', -1000)
        assert(player:getCurrency('zeni_point') == 0, 'addCurrency allowed zeni to become negative')

        player:addCurrency('zeni_point', 200, 150)
        assert(player:getCurrency('zeni_point') == 150, 'addCurrency did not honor explicit max')

        player:addCurrency('zeni_point', 10, 150)
        assert(player:getCurrency('zeni_point') == 150, 'addCurrency above explicit max changed capped zeni')

        player:addCurrency('zeni_point', 5, 'ignored')
        assert(player:getCurrency('zeni_point') == 155, 'addCurrency did not ignore non-numeric max object')

        player:delCurrency('zeni_point', 40)
        assert(player:getCurrency('zeni_point') == 115, 'delCurrency did not subtract zeni')

        player:delCurrency('zeni_point', 1000)
        assert(player:getCurrency('zeni_point') == 0, 'delCurrency allowed zeni to become negative')

        player:delCurrency('zeni_point', -30)
        assert(player:getCurrency('zeni_point') == 30, 'delCurrency with negative amount did not add zeni')

        player:setCurrency('zeni_point', -10)
        assert(player:getCurrency('zeni_point') == -10, 'setCurrency did not preserve a signed amount')

        player:addCurrency('zeni_point', 5)
        assert(player:getCurrency('zeni_point') == 0, 'addCurrency from negative balance did not clamp at zero')

        player:setCurrency('beastman_seal', 12)
        assert(player:getSeals(0) == 12, 'setCurrency did not update shared beastman seal balance')

        player:addCurrency('beastman_seal', 5)
        assert(player:getSeals(0) == 17, 'addCurrency did not update shared beastman seal balance')

        player:delCurrency('beastman_seal', 7)
        assert(player:getSeals(0) == 10, 'delCurrency did not update shared beastman seal balance')

        player:setCurrency('sandoria_cp', 20)
        assert(player:getCP() == 20, 'setCurrency did not update shared San dOria conquest point balance')

        player:addCurrency('sandoria_cp', 5)
        assert(player:getCP() == 25, 'addCurrency did not update shared San dOria conquest point balance')

        player:delCurrency('sandoria_cp', 7)
        assert(player:getCP() == 18, 'delCurrency did not update shared San dOria conquest point balance')

        player:setCurrency('leujaoam_assault_point', 31)
        assert(player:getAssaultPoint(0) == 31, 'setCurrency did not update shared Leujaoam assault point balance')

        player:addCurrency('leujaoam_assault_point', 6)
        assert(player:getAssaultPoint(0) == 37, 'addCurrency did not update shared Leujaoam assault point balance')

        player:delCurrency('leujaoam_assault_point', 12)
        assert(player:getAssaultPoint(0) == 25, 'delCurrency did not update shared Leujaoam assault point balance')

        player:setCurrency('current_accolades', 5)
        player:addCurrency('unity_accolades', 40)
        assert(player:getCurrency('unity_accolades') == 40, 'addCurrency did not update unity accolades')
        assert(player:getCurrency('current_accolades') == 45, 'addCurrency did not update current accolades side balance')

        player:delCurrency('unity_accolades', -15)
        assert(player:getCurrency('unity_accolades') == 55, 'delCurrency with negative amount did not update unity accolades')
        assert(player:getCurrency('current_accolades') == 60, 'delCurrency with negative amount did not update current accolades side balance')

        player:setCurrency('zeni_typo', 999)
        player:addCurrency('zeni_typo', 999)
        player:delCurrency('zeni_typo', -999)
        assert(player:getCurrency('zeni_typo') == 0, 'Invalid currency type became readable')

        npc:setCurrency('zeni_point', 1000)
        mob:setCurrency('zeni_point', 1000)
        npc:addCurrency('zeni_point', 1000)
        mob:addCurrency('zeni_point', 1000)
        npc:delCurrency('zeni_point', -1000)
        mob:delCurrency('zeni_point', -1000)
        assert(npc:getCurrency('zeni_point') == 0, 'NPC currency fallback changed')
        assert(mob:getCurrency('zeni_point') == 0, 'Mob currency fallback changed')

        assert(not pcall(function()
            player:getCurrency(123)
        end), 'getCurrency accepted a non-string type')

        assert(not pcall(function()
            player:addCurrency('zeni_point', 'bad')
        end), 'addCurrency accepted a non-numeric amount')

        assert(not pcall(function()
            player:setCurrency('zeni_point', 'bad')
        end), 'setCurrency accepted a non-numeric amount')

        assert(not pcall(function()
            player:delCurrency('zeni_point', 'bad')
        end), 'delCurrency accepted a non-numeric amount')
    end)
end)
