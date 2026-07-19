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
