require('scripts/globals/caskets')

describe('Treasure Casket trade routing', function()
    local function chest(vars)
        return {
            getLocalVar = function(_, name) return vars[name] end,
            setLocalVar = function(_, name, value) vars[name] = value end,
        }
    end

    it('first eligible Thief-tools trade grants a range hint and confirms the trade', function()
        local vars = {
            ['[caskets]LOCKED'] = 1,
            ['[caskets]CORRECT_NUM'] = 45,
            ['[caskets]PARTYID'] = 77,
            ['[caskets]HINT_TRADE'] = 0,
        }
        local removed, confirmed, message
        local oldTradeCheck = npcUtil.tradeHasExactly
        local oldRandom = math.random
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        npcUtil.tradeHasExactly = function(_, item)
            return item == xi.item.SET_OF_THIEFS_TOOLS
        end
        local rangeRolls = { 2, 4 }
        math.random = function(low, high)
            if low == nil then
                return 0.9
            end
            assert(low == 1 and high == 9)
            return table.remove(rangeRolls, 1)
        end
        local player = {
            getZoneID = function() return 999 end,
            getLeaderID = function() return 77 end,
            getMainJob = function() return xi.job.THF end,
            delStatusEffect = function(_, effect) removed = effect end,
            messageSpecial = function(_, ...) message = { ... } end,
            confirmTrade = function() confirmed = true end,
        }

        xi.caskets.onTrade(player, chest(vars), {})

        math.random = oldRandom
        npcUtil.tradeHasExactly = oldTradeCheck
        zones[999] = oldZone
        assert(removed == xi.effect.INVISIBLE and confirmed)
        assert(vars['[caskets]HINT_TRADE'] == 1)
        assert(message[1] == 1013 and message[2] == 32 and message[3] == 54)
    end)

    it('a repeat trade at the 20 percent cutoff confirms tools but gives no hint', function()
        local vars = {
            ['[caskets]LOCKED'] = 1,
            ['[caskets]CORRECT_NUM'] = 45,
            ['[caskets]PARTYID'] = 77,
            ['[caskets]HINT_TRADE'] = 1,
        }
        local confirmed, message
        local oldTradeCheck = npcUtil.tradeHasExactly
        local oldRandom = math.random
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        npcUtil.tradeHasExactly = function() return true end
        math.random = function() return 0.2 end
        local player = {
            getZoneID = function() return 999 end,
            getLeaderID = function() return 77 end,
            getMainJob = function() return xi.job.THF end,
            delStatusEffect = function() end,
            messageSpecial = function(_, ...) message = { ... } end,
            confirmTrade = function() confirmed = true end,
        }

        xi.caskets.onTrade(player, chest(vars), {})

        math.random = oldRandom
        npcUtil.tradeHasExactly = oldTradeCheck
        zones[999] = oldZone
        assert(confirmed and vars['[caskets]HINT_TRADE'] == 1)
        assert(message[1] == 1019 and message[2] == 0)
    end)

    it('removes invisibility but rejects a non-owner before checking the trade', function()
        local vars = {
            ['[caskets]LOCKED'] = 1,
            ['[caskets]CORRECT_NUM'] = 45,
            ['[caskets]PARTYID'] = 77,
        }
        local removed, tradeChecks, confirmed = nil, 0, false
        local oldTradeCheck = npcUtil.tradeHasExactly
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        npcUtil.tradeHasExactly = function() tradeChecks = tradeChecks + 1 return true end
        local player = {
            getZoneID = function() return 999 end,
            getLeaderID = function() return 66 end,
            getMainJob = function() return xi.job.THF end,
            delStatusEffect = function(_, effect) removed = effect end,
            confirmTrade = function() confirmed = true end,
        }

        xi.caskets.onTrade(player, chest(vars), {})

        npcUtil.tradeHasExactly = oldTradeCheck
        zones[999] = oldZone
        assert(removed == xi.effect.INVISIBLE and tradeChecks == 0 and not confirmed)
    end)
end)
