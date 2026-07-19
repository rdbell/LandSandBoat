require('scripts/globals/brigands_chart')

describe("Brigand's Chart trade", function()
    it('admits an exact chart trade at an unclaimed normal question mark', function()
        local message = nil
        local event = nil
        local confirmedItem = nil
        local confirmedQuantity = nil
        local player = {
            messageSpecial = function(_, ...) message = { ... } end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = {
            getStatus = function() return xi.status.NORMAL end,
            getLocalVar = function(_, name) return name == 'bChartSpawnerID' and 0 or 0 end,
        }
        local trade = {
            getSlotCount = function() return 1 end,
            getItemId = function() return xi.item.BRIGANDS_CHART end,
            getItemQty = function(_, item) return item == xi.item.BRIGANDS_CHART and 1 or 0 end,
            confirmItem = function(_, item, quantity)
                confirmedItem = item
                confirmedQuantity = quantity
            end,
        }

        xi.brigandsChart.onTrade(player, npc, trade)

        assert(confirmedItem == xi.item.BRIGANDS_CHART and confirmedQuantity == 1)
        assert(message[1] == 7850 and message[2] == xi.item.BRIGANDS_CHART)
        assert(event[1] == 902)
    end)
end)
