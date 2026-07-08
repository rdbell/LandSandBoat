describe('Base entity character variable helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    local function tableCount(tableValue)
        local count = 0

        for _ in pairs(tableValue) do
            count = count + 1
        end

        return count
    end

    it('reads and updates player character variables while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local now = GetSystemTime()

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getCharVar('missing') == 0, 'Missing char var was not zero')

        player:setCharVar('[OMEGA]Alpha', 10)
        assert(player:getCharVar('[OMEGA]Alpha') == 10, 'setCharVar did not store a value')
        assert(player:getVar('[OMEGA]Alpha') == 10, 'getVar alias did not read a value')

        player:setVar('[OMEGA]Alias', 11)
        assert(player:getCharVar('[OMEGA]Alias') == 11, 'setVar alias did not store a value')

        player:incrementCharVar('[OMEGA]Alpha', 5)
        assert(player:getCharVar('[OMEGA]Alpha') == 15, 'incrementCharVar did not add to an existing value')

        player:incrementCharVar('[OMEGA]Created', -3)
        assert(player:getCharVar('[OMEGA]Created') == -3, 'incrementCharVar did not create a missing value')

        player:setCharVar('[OMEGA]ToZero', 3)
        player:incrementCharVar('[OMEGA]ToZero', -3)
        assert(player:getCharVar('[OMEGA]ToZero') == 0, 'incrementCharVar did not allow a zero result')

        player:setVolatileCharVar('[OMEGA]Volatile', 22)
        assert(player:getCharVar('[OMEGA]Volatile') == 22, 'setVolatileCharVar did not update the readable cache')

        player:setCharVar('[OMEGA]Future', 33, now + 3600)
        assert(player:getCharVar('[OMEGA]Future') == 33, 'setCharVar with future expiry did not store a value')

        player:setCharVar('[OMEGA]ExpiredWrite', 44, now - 1)
        assert(player:getCharVar('[OMEGA]ExpiredWrite') == 0, 'setCharVar accepted an expired timestamp')

        player:setCharVar('[OMEGA]ExpiryUpdate', 55)
        player:setCharVarExpiration('[OMEGA]ExpiryUpdate', now + 3600)
        assert(player:getCharVar('[OMEGA]ExpiryUpdate') == 55, 'setCharVarExpiration changed the stored value')

        player:setCharVarExpiration('[OMEGA]ExpiryUpdate', now - 1)
        assert(player:getCharVar('[OMEGA]ExpiryUpdate') == 55, 'setCharVarExpiration accepted an expired timestamp')

        player:setVolatileCharVar('[OMEGA]ExpiredVolatile', 66, now - 1)
        assert(player:getCharVar('[OMEGA]ExpiredVolatile') == 0, 'setVolatileCharVar accepted an expired timestamp')

        player:setCharVar('[OMEGA]DeleteMe', 77)
        player:setCharVar('[OMEGA]DeleteMe', 0)
        assert(player:getCharVar('[OMEGA]DeleteMe') == 0, 'setCharVar with zero did not delete the value')

        local prefixVars = player:getCharVarsWithPrefix('[OMEGA]')
        assert(prefixVars['[OMEGA]Alpha'] == 15, 'getCharVarsWithPrefix missed Alpha')
        assert(prefixVars['[OMEGA]Alias'] == 11, 'getCharVarsWithPrefix missed Alias')
        assert(prefixVars['[OMEGA]ToZero'] == 0, 'getCharVarsWithPrefix missed zero-valued increment row')
        assert(prefixVars['[OMEGA]Volatile'] == nil, 'getCharVarsWithPrefix included volatile-only cache state')
        assert(prefixVars['[OMEGA]ExpiredWrite'] == nil, 'getCharVarsWithPrefix included expired write')

        local suffixVars = player:getCharVarsWithSuffix('Created')
        assert(suffixVars['[OMEGA]Created'] == -3, 'getCharVarsWithSuffix missed Created')

        player:clearVarsWithPrefix('bad')
        assert(player:getCharVar('[OMEGA]Alpha') == 15, 'short clearVarsWithPrefix changed variables')

        player:clearVarsWithPrefix('[OMEGA]')
        assert(player:getCharVar('[OMEGA]Alpha') == 0, 'clearVarsWithPrefix did not clear Alpha')
        assert(player:getCharVar('[OMEGA]Created') == 0, 'clearVarsWithPrefix did not clear Created')
        assert(player:getCharVar('[OMEGA]Volatile') == 0, 'clearVarsWithPrefix did not clear volatile-only cache state')

        npc:setCharVar('[OMEGA]Npc', 100)
        mob:setCharVar('[OMEGA]Mob', 100)
        npc:incrementCharVar('[OMEGA]Npc', 100)
        mob:incrementCharVar('[OMEGA]Mob', 100)
        npc:setVolatileCharVar('[OMEGA]NpcVolatile', 100)
        mob:setVolatileCharVar('[OMEGA]MobVolatile', 100)
        npc:clearVarsWithPrefix('[OMEGA]')
        mob:clearVarsWithPrefix('[OMEGA]')
        assert(npc:getCharVar('[OMEGA]Npc') == 0, 'NPC char var fallback changed')
        assert(mob:getCharVar('[OMEGA]Mob') == 0, 'Mob char var fallback changed')
        assert(tableCount(npc:getCharVarsWithPrefix('[OMEGA]')) == 0, 'NPC prefix table was not empty')
        assert(tableCount(mob:getCharVarsWithSuffix('Mob')) == 0, 'Mob suffix table was not empty')

        assert(not pcall(function()
            player:getCharVar()
        end), 'getCharVar accepted missing args')

        assert(not pcall(function()
            player:setCharVar('[OMEGA]Bad')
        end), 'setCharVar accepted missing value')

        assert(not pcall(function()
            player:setCharVarExpiration('[OMEGA]Bad')
        end), 'setCharVarExpiration accepted missing expiry')

        assert(not pcall(function()
            player:incrementCharVar('[OMEGA]Bad')
        end), 'incrementCharVar accepted missing value')

        assert(not pcall(function()
            player:setVolatileCharVar('[OMEGA]Bad')
        end), 'setVolatileCharVar accepted missing value')
    end)
end)
