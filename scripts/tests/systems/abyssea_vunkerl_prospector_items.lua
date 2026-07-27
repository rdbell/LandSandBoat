require('scripts/globals/abyssea')

describe('Abyssea Vunkerl Cruor Prospector permanent items', function()
    it('offers its regular item and Forbidden Key selections', function()
        local granted, removed = {}, {}
        local player = { getCurrency = function() return 5000 end, delCurrency = function(_, _, amount) removed[#removed + 1] = amount end }
        local oldGiveItem = npcUtil.giveItem
        npcUtil.giveItem = function(_, items) granted[#granted + 1] = items[1]; return true end
        local npc = require('scripts/zones/Abyssea-Vunkerl/npcs/Cruor_Prospector')
        npc.onEventFinish(player, 0, xi.abyssea.itemType.ITEM + 65536)
        npc.onEventFinish(player, 0, xi.abyssea.itemType.ITEM + 10 * 65536 + 2 * 16777216)
        npcUtil.giveItem = oldGiveItem
        assert(granted[1][1] == xi.item.UNKAI_KABUTO and granted[1][2] == 1)
        assert(granted[2][1] == xi.item.FORBIDDEN_KEY and granted[2][2] == 2)
        assert(removed[1] == 5000 and removed[2] == 1000)
    end)
end)
