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

    it('grants one temporary item and charges its fixed cost', function()
        local granted = nil
        local removed = nil
        local player = {
            getCurrency = function() return 80 end,
            delCurrency = function(_, currency, amount) removed = { currency, amount } end,
        }
        local oldGiveTempItem = npcUtil.giveTempItem
        npcUtil.giveTempItem = function(_, items)
            granted = items
            return true
        end

        xi.abyssea.visionsCruorProspectorOnEventFinish(
            player,
            0,
            xi.abyssea.itemType.TEMP + 65536 + 99 * 16777216,
            { [xi.abyssea.itemType.TEMP] = { [1] = { xi.item.LUCID_POTION_I, 80 } } }
        )

        npcUtil.giveTempItem = oldGiveTempItem
        assert(granted[1][1] == xi.item.LUCID_POTION_I and granted[1][2] == 1)
        assert(removed[1] == 'cruor' and removed[2] == 80)
    end)

    it('does not give or charge a temporary item without cruor or a successful grant', function()
        local granted = false
        local removed = false
        local player = {
            getCurrency = function() return 79 end,
            delCurrency = function() removed = true end,
        }
        local oldGiveTempItem = npcUtil.giveTempItem
        npcUtil.giveTempItem = function()
            granted = true
            return false
        end

        xi.abyssea.visionsCruorProspectorOnEventFinish(
            player,
            0,
            xi.abyssea.itemType.TEMP + 65536,
            { [xi.abyssea.itemType.TEMP] = { [1] = { xi.item.LUCID_POTION_I, 80 } } }
        )

        assert(not granted and not removed)

        player.getCurrency = function() return 80 end
        xi.abyssea.visionsCruorProspectorOnEventFinish(
            player,
            0,
            xi.abyssea.itemType.TEMP + 65536,
            { [xi.abyssea.itemType.TEMP] = { [1] = { xi.item.LUCID_POTION_I, 80 } } }
        )
        npcUtil.giveTempItem = oldGiveTempItem
        assert(granted and not removed)
    end)
end)
