require('scripts/globals/abyssea')

describe('Abyssea cruor prospector purchase', function()
    it('uses one regular item despite packed quantity bits', function()
        local granted = nil
        local removed = nil
        local player = {
            getCurrency = function() return 4000 end,
            delCurrency = function(_, currency, amount) removed = { currency, amount } end,
        }
        local oldGiveItem = npcUtil.giveItem
        npcUtil.giveItem = function(_, items)
            granted = items
            return true
        end

        xi.abyssea.visionsCruorProspectorOnEventFinish(
            player,
            0,
            xi.abyssea.itemType.ITEM + 65536 + 99 * 16777216,
            { [xi.abyssea.itemType.ITEM] = { [1] = { xi.item.PERLE_SALADE, 4000 } } }
        )

        npcUtil.giveItem = oldGiveItem
        assert(granted[1][1] == xi.item.PERLE_SALADE and granted[1][2] == 1)
        assert(removed[1] == 'cruor' and removed[2] == 4000)
    end)

    it('uses Forbidden Key quantity bits and only charges after a successful grant', function()
        local granted = nil
        local removed = nil
        local player = {
            getCurrency = function() return 1500 end,
            delCurrency = function(_, currency, amount) removed = { currency, amount } end,
        }
        local oldGiveItem = npcUtil.giveItem
        npcUtil.giveItem = function(_, items)
            granted = items
            return true
        end

        xi.abyssea.visionsCruorProspectorOnEventFinish(
            player,
            0,
            xi.abyssea.itemType.ITEM + 16 * 65536 + 3 * 16777216,
            { [xi.abyssea.itemType.ITEM] = { [16] = { xi.item.FORBIDDEN_KEY, 500 } } }
        )

        npcUtil.giveItem = oldGiveItem
        assert(granted[1][1] == xi.item.FORBIDDEN_KEY and granted[1][2] == 3)
        assert(removed[1] == 'cruor' and removed[2] == 1500)
    end)

    it('does not give or charge when cruor is insufficient', function()
        local granted = false
        local removed = false
        local player = {
            getCurrency = function() return 1499 end,
            delCurrency = function() removed = true end,
        }
        local oldGiveItem = npcUtil.giveItem
        npcUtil.giveItem = function()
            granted = true
            return true
        end

        xi.abyssea.visionsCruorProspectorOnEventFinish(
            player,
            0,
            xi.abyssea.itemType.ITEM + 16 * 65536 + 3 * 16777216,
            { [xi.abyssea.itemType.ITEM] = { [16] = { xi.item.FORBIDDEN_KEY, 500 } } }
        )

        npcUtil.giveItem = oldGiveItem
        assert(not granted and not removed)
    end)
end)
