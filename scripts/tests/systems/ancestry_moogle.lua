require('scripts/globals/ancestry_moogle')

describe('Ancestry Moogle trigger', function()
    it('starts the race-change event for an eligible player', function()
        local event = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function(_, name)
                return name == '[RaceChange]Eligible' and 9999999999 or 0
            end,
            getRace = function() return xi.race.HUME_F end,
            getFace = function() return 5 end,
            getSize = function() return 2 end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = {
            getID = function() return 100 end,
        }

        xi.ancestryMoogle.onTrigger(player, npc)

        assert(event[1] == 479)
        assert(event[2] == xi.race.HUME_F and event[3] == 2 and event[4] == 2 and event[5] == 1)
        assert(event[6] > 0 and event[7] == 0 and event[8] == 0 and event[9] == 0)
    end)
end)
