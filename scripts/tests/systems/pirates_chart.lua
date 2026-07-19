require('scripts/globals/pirates_chart')

describe("Pirates Chart loot catalog", function()
    it('pins all four weighted loot groups', function()
        local loot = xi.piratesChart.loot
        assert(#loot == 4 and #loot[1] == 7 and #loot[2] == 7 and #loot[3] == 3 and #loot[4] == 1)
        assert(loot[1][1].itemId == xi.item.CORAL_FRAGMENT and loot[1][1].weight == xi.loot.weight.VERY_LOW)
        assert(loot[1][6].itemId == xi.item.SHALL_SHELL and loot[1][6].weight == xi.loot.weight.VERY_HIGH)
        assert(loot[3][2].itemId == xi.item.MERCURIAL_KRIS and loot[3][2].weight == xi.loot.weight.EXTREMELY_LOW)
        assert(loot[4][1].itemId == xi.item.ALBATROSS_RING and loot[4][1].weight == 1000)
    end)
end)

describe("Pirates Chart trade", function()
    it('starts the chart event and prioritizes the party-size rejection', function()
        local message, event = nil, nil
        local player = {
            getParty = function() return {} end,
            getPartySize = function() return 3 end,
            checkSoloPartyAlliance = function() return 0 end,
            messageSpecial = function(_, ...) message = { ... } end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = { getStatus = function() return xi.status.NORMAL end }
        local box = { getStatus = function() return xi.status.DISAPPEAR end }
        local trade = {
            getSlotCount = function() return 1 end,
            getItemId = function() return xi.item.PIRATES_CHART end,
            getItemQty = function() return 1 end,
            confirmItem = function() end,
        }
        stub('GetNPCByID', function() return box end)
        xi.piratesChart.onTrade(player, npc, trade)
        assert(message[1] == 7825 and message[2] == xi.item.PIRATES_CHART)
        assert(event[1] == 14 and event[5] == 3)

        player.getPartySize = function() return 4 end
        player.checkSoloPartyAlliance = function() return 2 end
        message, event = nil, nil
        xi.piratesChart.onTrade(player, npc, trade)
        assert(message[1] == 7845 and message[2] == 3 and event == nil)
    end)
end)
