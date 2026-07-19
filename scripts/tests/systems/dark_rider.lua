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

describe('Dark Rider hoofprint spawning', function()
    it('spawns the selected-zone hoofprint and increments its count', function()
        local status = xi.status.DISAPPEAR
        local position = nil
        local daysSinceEpoch = nil
        local count = nil
        local seed = 0
        local hoofprint = {
            getStatus = function() return status end,
            setPos = function(_, value) position = value end,
            setStatus = function(_, value) status = value end,
            setLocalVar = function(_, name, value)
                if name == 'DaysSinceEpoch' then
                    daysSinceEpoch = value
                end
            end,
        }
        local zone = {
            getID = function() return xi.zone.WAJAOM_WOODLANDS end,
            getLocalVar = function(_, name) return name == 'HoofprintCount' and 0 or 0 end,
            setLocalVar = function(_, name, value)
                if name == 'HoofprintCount' then
                    count = value
                end
            end,
        }

        stub('VanadielMoonPhase', function() return seed end)
        stub('VanadielDayElement', 0)
        stub('VanadielDayOfTheMonth', 0)
        stub('VanadielDayOfTheYear', 0)
        stub('VanadielUniqueDay', 11)
        stub('GetNPCByID', hoofprint)

        -- hoofprintZones is built with pairs(), so its local iteration order
        -- is intentionally unspecified. One complete modulus cycle must
        -- nevertheless select Wajaom exactly once.
        for i = 0, 3 do
            seed = i
            xi.darkRider.addHoofprints(zone)
        end

        assert(position ~= nil)
        assert(status == xi.status.NORMAL)
        assert(daysSinceEpoch == 11)
        assert(count == 1)
    end)
end)
