require('scripts/globals/caskets')

describe('Treasure Casket drop assignment', function()
    local function chest(vars)
        local zone = { queryEntitiesByName = function() return { { getID = function() return 100 end } } end }
        return {
            getID = function() return 100 end,
            getZone = function() return zone end,
            getLocalVar = function(_, name) return vars[name] end,
            setLocalVar = function(_, name, value) vars[name] = value end,
            setAnimationSub = function() end,
        }
    end

    it('assigns the requested number of temporary drops through onTrigger', function()
        local vars = { ['[caskets]LOOT_TYPE'] = 1, ['[caskets]LOCKED'] = 0, ['[caskets]PARTYID'] = 77, ['[caskets]SPAWNSTATUS'] = 2, ['[caskets]ITEMS_SET'] = 0, ['[caskets]MOBLVL'] = 1, ['[caskets]ATTEMPTS'] = 0, ['[caskets]FAILED_ATEMPTS'] = 0 }
        local oldEntry, oldPick = utils.randomEntry, xi.itemUtils.pickItemRandom
        utils.randomEntry = function() return 2 end
        local picks = { 11, 22 }
        xi.itemUtils.pickItemRandom = function() return table.remove(picks, 1) end
        local player = { getLeaderID = function() return 77 end, getZoneID = function() return 172 end, startEvent = function() end }

        xi.caskets.onTrigger(player, chest(vars))

        utils.randomEntry, xi.itemUtils.pickItemRandom = oldEntry, oldPick
        assert(vars['[caskets]TEMP1'] == 11 and vars['[caskets]TEMP2'] == 22 and vars['[caskets]TEMP3'] == 0 and vars['[caskets]ITEMS_SET'] == 1)
    end)

    it('uses a regional replacement in slot one for item drops', function()
        local vars = { ['[caskets]LOOT_TYPE'] = 2, ['[caskets]LOCKED'] = 0, ['[caskets]PARTYID'] = 77, ['[caskets]SPAWNSTATUS'] = 2, ['[caskets]ITEMS_SET'] = 0, ['[caskets]MOBLVL'] = 1, ['[caskets]ATTEMPTS'] = 0, ['[caskets]FAILED_ATEMPTS'] = 0 }
        local oldEntry, oldPick, oldRandom = utils.randomEntry, xi.itemUtils.pickItemRandom, math.random
        local entryCalls = 0
        utils.randomEntry = function()
            entryCalls = entryCalls + 1
            return entryCalls == 1 and 2 or 999
        end
        local picks = { 101, 202 }
        xi.itemUtils.pickItemRandom = function() return table.remove(picks, 1) end
        local rolls = { 6, 5 }
        math.random = function(low, high) assert(low == 1 and high == 100); return table.remove(rolls, 1) end
        local player = { getLeaderID = function() return 77 end, getZoneID = function() return 172 end, startEvent = function() end }

        xi.caskets.onTrigger(player, chest(vars))

        utils.randomEntry, xi.itemUtils.pickItemRandom, math.random = oldEntry, oldPick, oldRandom
        assert(vars['[caskets]ITEM1'] == 999 and vars['[caskets]ITEM2'] == 0 and vars['[caskets]ITEMS_SET'] == 1)
    end)
end)
