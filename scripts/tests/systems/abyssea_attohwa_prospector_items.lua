require('scripts/globals/abyssea')

describe('Abyssea Attohwa Cruor Prospector permanent items', function()
    it('offers its regular item and Forbidden Key selections', function()
        local granted = {}
        local removed = {}
        local player = {
            getCurrency = function() return 5000 end,
            delCurrency = function(_, currency, amount) removed[#removed + 1] = { currency, amount } end,
        }
        local oldGiveItem = npcUtil.giveItem
        npcUtil.giveItem = function(_, items)
            granted[#granted + 1] = items[1]
            return true
        end
        local npc = require('scripts/zones/Abyssea-Attohwa/npcs/Cruor_Prospector')

        npc.onEventFinish(player, 0, xi.abyssea.itemType.ITEM + 65536)
        npc.onEventFinish(player, 0, xi.abyssea.itemType.ITEM + 7 * 65536 + 3 * 16777216)

        npcUtil.giveItem = oldGiveItem
        assert(granted[1][1] == xi.item.RAVAGERS_MASK and granted[1][2] == 1)
        assert(granted[2][1] == xi.item.FORBIDDEN_KEY and granted[2][2] == 3)
        assert(removed[1][1] == 'cruor' and removed[1][2] == 5000)
        assert(removed[2][1] == 'cruor' and removed[2][2] == 1500)
    end)
end)
