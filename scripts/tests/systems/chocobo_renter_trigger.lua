describe('Chocobo renter trigger', function()
    local function player(zone, level, license, mission, gil, notes)
        local event
        local price
        return {
            getZoneID = function() return zone end,
            getMainLvl = function() return level end,
            hasKeyItem = function(_, key) assert(key == xi.ki.CHOCOBO_LICENSE); return license end,
            hasCompletedMission = function() return mission end,
            getGil = function() return gil end,
            getCurrency = function(_, currency) assert(currency == 'allied_notes'); return notes end,
            setLocalVar = function(_, key, value) assert(key == '[CHOCOBO]price'); price = value end,
            startEvent = function(_, ...) event = { ... } end,
            event = function() return event end,
            price = function() return price end,
        }
    end

    it('selects failure, race, and normal-rental events from injected player state', function()
        local originalRaceCheck = xi.chocoboGame.raceCheck
        local originalStartRace = xi.chocoboGame.startRaceEvent
        local route
        xi.chocoboGame.raceCheck = function() return false end
        xi.chocoboGame.startRaceEvent = function(_, destination, eventSucceed)
            route = { destination, eventSucceed }
        end

        local ok, err = pcall(function()
            local city = player(xi.zone.WINDURST_WOODS, 15, true, false, 99, 0)
            xi.chocobo.renterOnTrigger(city, {}, 100, 200)
            assert(city.price() == 65 and city.event()[1] == 100 and city.event()[2] == 65 and city.event()[3] == 99 and city.event()[4] == 1)

            local past = player(xi.zone.SOUTHERN_SAN_DORIA_S, 15, true, true, 99, 77)
            xi.chocobo.renterOnTrigger(past, {}, 100, 200)
            assert(past.event()[1] == 100 and past.event()[2] == 65 and past.event()[3] == 77 and past.event()[4] == 1)

            local denied = player(xi.zone.WINDURST_WOODS, 99, false, false, 99, 0)
            xi.chocobo.renterOnTrigger(denied, {}, 100, 200)
            assert(denied.event()[1] == 200)

            xi.chocoboGame.raceCheck = function() return xi.zone.SAUROMUGUE_CHAMPAIGN end
            local racer = player(xi.zone.WINDURST_WOODS, 20, true, false, 99, 0)
            xi.chocobo.renterOnTrigger(racer, {}, 100, 200)
            assert(route[1] == xi.zone.SAUROMUGUE_CHAMPAIGN and route[2] == 100 and racer.event() == nil)
        end)

        xi.chocoboGame.raceCheck = originalRaceCheck
        xi.chocoboGame.startRaceEvent = originalStartRace
        assert(ok, err)
    end)
end)
