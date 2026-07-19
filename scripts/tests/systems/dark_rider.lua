require('scripts/globals/dark_rider')

describe('Dark Rider hoofprint cleanup', function()
    it('hides stale hoofprints at 06:00 and decrements the zone count', function()
        local status = xi.status.NORMAL
        local reset = false
        local count = nil
        local hoofprint = {
            getStatus = function() return status end,
            getLocalVar = function(_, name) return name == 'DaysSinceEpoch' and 10 or 0 end,
            setStatus = function(_, value) status = value end,
            resetLocalVars = function() reset = true end,
        }
        local zone = {
            getID = function() return xi.zone.WAJAOM_WOODLANDS end,
            getLocalVar = function(_, name) return name == 'HoofprintCount' and 1 or 0 end,
            setLocalVar = function(_, name, value)
                if name == 'HoofprintCount' then
                    count = value
                end
            end,
        }

        stub('VanadielHour', 6)
        stub('VanadielUniqueDay', 11)
        stub('GetNPCByID', hoofprint)

        xi.darkRider.onGameHour(zone)

        assert(status == xi.status.DISAPPEAR)
        assert(reset)
        assert(count == 0)
    end)
end)
