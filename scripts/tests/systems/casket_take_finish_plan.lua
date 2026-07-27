require('scripts/globals/caskets')

describe('Treasure Casket unlocked take routing', function()
    local function chest(vars, calls)
        return {
            getLocalVar = function(_, name) return vars[name] end,
            setLocalVar = function(_, name, value) vars[name] = value calls.set[#calls.set + 1] = { name, value } end,
            setAnimationSub = function(_, ...) calls.animation = { ... } end,
            setStatus = function(_, status) calls.status = status end,
            resetLocalVars = function() calls.reset = true end,
        }
    end

    local function playerFor(casket, calls)
        local player
        player = {
            getZoneID = function() return 999 end,
            getEventTarget = function() return casket end,
            getAlliance = function() return { player } end,
            messageSpecial = function(_, ...) calls.messages[#calls.messages + 1] = { ... } end,
            messageName = function(_, ...) calls.broadcasts[#calls.broadcasts + 1] = { ... } end,
        }
        return player
    end

    it('grants and clears the final temporary item then removes the casket', function()
        local vars = { ['[caskets]SPAWNSTATUS'] = 2, ['[caskets]LOCKED'] = 0, ['[caskets]LOOT_TYPE'] = 1, ['[caskets]ATTEMPTS'] = 0, ['[caskets]FAILED_ATEMPTS'] = 0, ['[caskets]TEMP1'] = 11, ['[caskets]TEMP2'] = 0, ['[caskets]TEMP3'] = 0 }
        local calls = { set = {}, messages = {}, broadcasts = {} }
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000, PLAYER_OBTAINS_ITEM = 2000, UNABLE_TO_OBTAIN_ITEM = 3000, ALREADY_POSSESS_TEMP = 3001, ITEM_CANNOT_BE_OBTAINED = 3002 } }
        local casket = chest(vars, calls)
        local player = playerFor(casket, calls)
        player.hasItem = function() return false end
        player.addTempItem = function(_, id) calls.tempAdded = id return true end

        xi.caskets.onEventFinish(player, 0, bit.lshift(1, 16) + 1, nil)

        zones[999] = oldZone
        assert(calls.tempAdded == 11 and vars['[caskets]TEMP1'] == 0)
        assert(calls.broadcasts[1][1] == 1000 and calls.broadcasts[1][3] == 11)
        assert(calls.animation[1] == 0 and calls.status == xi.status.DISAPPEAR and calls.reset)
    end)

    it('grants a multiple item with quantity 33 and retains a nonempty casket', function()
        local vars = { ['[caskets]SPAWNSTATUS'] = 2, ['[caskets]LOCKED'] = 0, ['[caskets]LOOT_TYPE'] = 2, ['[caskets]ATTEMPTS'] = 0, ['[caskets]FAILED_ATEMPTS'] = 0, ['[caskets]ITEM1'] = xi.item.HANDFUL_OF_STONE_ARROWHEADS, ['[caskets]ITEM2'] = 200, ['[caskets]ITEM3'] = 0, ['[caskets]ITEM4'] = 0 }
        local calls = { set = {}, messages = {}, broadcasts = {} }
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000, PLAYER_OBTAINS_ITEM = 2000, UNABLE_TO_OBTAIN_ITEM = 3000, ALREADY_POSSESS_TEMP = 3001, ITEM_CANNOT_BE_OBTAINED = 3002 } }
        local casket = chest(vars, calls)
        local player = playerFor(casket, calls)
        player.getFreeSlotsCount = function() return 1 end
        player.addItem = function(_, id, qty) calls.itemAdded = { id, qty } return true end

        xi.caskets.onEventFinish(player, 0, bit.lshift(1, 16) + 1, nil)

        zones[999] = oldZone
        assert(calls.itemAdded[1] == xi.item.HANDFUL_OF_STONE_ARROWHEADS and calls.itemAdded[2] == 33)
        assert(vars['[caskets]ITEM1'] == 0 and vars['[caskets]ITEM2'] == 200 and not calls.reset)
        assert(calls.broadcasts[1][1] == 2000 and calls.broadcasts[1][3] == xi.item.HANDFUL_OF_STONE_ARROWHEADS)
    end)

    it('reports an already-held temporary item without changing the casket', function()
        local vars = { ['[caskets]SPAWNSTATUS'] = 2, ['[caskets]LOCKED'] = 0, ['[caskets]LOOT_TYPE'] = 1, ['[caskets]ATTEMPTS'] = 0, ['[caskets]FAILED_ATEMPTS'] = 0, ['[caskets]TEMP1'] = 11 }
        local calls = { set = {}, messages = {}, broadcasts = {} }
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000, PLAYER_OBTAINS_ITEM = 2000, UNABLE_TO_OBTAIN_ITEM = 3000, ALREADY_POSSESS_TEMP = 3001, ITEM_CANNOT_BE_OBTAINED = 3002 } }
        local casket = chest(vars, calls)
        local player = playerFor(casket, calls)
        player.hasItem = function() return true end

        xi.caskets.onEventFinish(player, 0, bit.lshift(1, 16) + 1, nil)

        zones[999] = oldZone
        assert(calls.messages[1][1] == 3001 and vars['[caskets]TEMP1'] == 11 and #calls.set == 0)
    end)
end)
