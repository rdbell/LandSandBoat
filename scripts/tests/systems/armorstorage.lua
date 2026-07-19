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
