require('scripts/globals/maps')

describe('Map vendor trigger', function()
    it('starts the vendor event with owned-map bit parameters', function()
        local event = nil
        local player = {
            hasKeyItem = function(_, keyItem) return keyItem == xi.ki.MAP_OF_THE_SAN_DORIA_AREA end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = {
            getName = function() return 'Ashu_Bolkhomo' end,
        }

        xi.maps.onTrigger(player, npc)

        assert(event[1] == 1006 and event[2] == 1 and event[3] == 0 and event[4] == 0)
    end)
end)

describe('Map vendor update', function()
    it('charges for a valid map, grants its key item, and refreshes the menu', function()
        local charged, granted, update = nil, nil, nil
        local player = {
            getGil = function() return 200 end,
            delGil = function(_, amount) charged = amount end,
            hasKeyItem = function() return false end,
            updateEvent = function(_, ...) update = { ... } end,
        }
        local npc = {
            getName = function() return 'Ashu_Bolkhomo' end,
        }

        local originalGiveKeyItem = npcUtil.giveKeyItem
        npcUtil.giveKeyItem = function(_, keyItem) granted = keyItem end
        xi.maps.onEventUpdate(player, 1006, 1, npc)
        npcUtil.giveKeyItem = originalGiveKeyItem

        assert(charged == 200 and granted == xi.ki.MAP_OF_THE_SAN_DORIA_AREA)
        assert(update[1] == 0 and update[2] == 0 and update[3] == 0)
    end)
end)
