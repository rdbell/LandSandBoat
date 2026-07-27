require('scripts/globals/abyssea')

describe('Abyssea exit maw trigger', function()
    local function player(zone)
        local events = {}
        return {
            getZoneID = function() return zone end,
            startEvent = function(_, ...) table.insert(events, { ... }) end,
            events = function() return events end,
        }
    end

    it('starts the exit event in an exit-maw zone', function()
        local p = player(xi.zone.ABYSSEA_ALTEPA)
        xi.abyssea.exitMawOnTrigger(p)
        assert(p.events()[1][1] == 200)
    end)

    it('does not start an event outside an exit-maw zone', function()
        local p = player(xi.zone.BASTOK_MARKETS)
        xi.abyssea.exitMawOnTrigger(p)
        assert(#p.events() == 0)
    end)
end)
