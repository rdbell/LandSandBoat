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
