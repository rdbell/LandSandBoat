require('scripts/globals/abyssea')

describe('Abyssea entrance maw trigger', function()
    local function player(zone, level)
        local events = {}
        local messages = 0
        return {
            getZoneID = function() return zone end,
            getMainLvl = function() return level end,
            startEvent = function(_, ...) table.insert(events, { ... }) end,
            messageSpecial = function() messages = messages + 1 end,
            events = function() return events end,
            messages = function() return messages end,
        }
    end

    it('starts the route event for an eligible player', function()
        local p = player(xi.zone.LA_THEINE_PLATEAU, 30)
        xi.abyssea.entranceMawOnTrigger(p)
        assert(p.events()[1][1] == 218 and p.events()[1][2] == 0 and p.events()[1][3] == 1)
    end)

    it('rejects a low-level player', function()
        local p = player(xi.zone.LA_THEINE_PLATEAU, 29)
        xi.abyssea.entranceMawOnTrigger(p)
        assert(#p.events() == 0 and p.messages() == 1)
    end)
end)
