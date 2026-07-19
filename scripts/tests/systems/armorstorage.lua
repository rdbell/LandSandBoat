require('scripts/globals/armorstorage')

describe('Armor Storage trigger', function()
    it('starts withdrawal with masks for stored armor sets', function()
        local event = nil
        local player = {
            hasKeyItem = function(_, keyItem)
                return keyItem == xi.ki.FIGHTERS_ARMOR_CLAIM_SLIP or keyItem == xi.ki.IRON_SCALE_ARMOR_CLAIM_SLIP
            end,
            getGil = function() return 1234 end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.armorStorage.onTrigger(player, 900)

        assert(event[1] == 900)
        assert(event[2] == 1 and event[3] == 0 and event[4] == 0)
        assert(event[5] == 1 and event[6] == 1234 and event[7] == 0)
    end)
end)

describe('Armor Storage update', function()
    it('updates the withdrawal menu with the selected set details', function()
        local event = nil
        local player = {
            updateEvent = function(_, ...) event = { ... } end,
        }

        xi.armorStorage.onEventUpdate(player, 900, 1, 900)

        assert(event[1] == 12511 and event[2] == 12638 and event[3] == 13961)
        assert(event[4] == 14214 and event[5] == 14089 and event[6] == 500)
    end)
end)

describe('Armor Storage withdrawal finish', function()
    it('restores a stored set when it has slots and gil', function()
        local items = {}
        local removedKeyItem = nil
        local gil = nil
        local player = {
            hasKeyItem = function(_, keyItem) return keyItem == xi.ki.FIGHTERS_ARMOR_CLAIM_SLIP end,
            getFreeSlotsCount = function() return 5 end,
            getGil = function() return 500 end,
            addItem = function(_, item) table.insert(items, item) end,
            messageSpecial = function() end,
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            delKeyItem = function(_, keyItem) removedKeyItem = keyItem end,
            setGil = function(_, amount) gil = amount end,
        }

        xi.armorStorage.onEventFinish(player, 900, 1, 800, 900)

        assert(#items == 5 and items[1] == 12511 and items[5] == 14089)
        assert(removedKeyItem == xi.ki.FIGHTERS_ARMOR_CLAIM_SLIP and gil == 0)
    end)
end)
