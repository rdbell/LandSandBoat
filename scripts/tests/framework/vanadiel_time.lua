describe('Vanadiel Time', function()
    local vanaEpoch = 1009810800

    local function assertEqual(actual, expected, label)
        assert(actual == expected, string.format('%s: expected %d, got %d', label, expected, actual))
    end

    local function setEarthTime(target)
        for _ = 1, 5 do
            SetTimeOffset(0)
            SetTimeOffset(target - GetSystemTime())

            if GetSystemTime() == target then
                return
            end
        end

        assertEqual(GetSystemTime(), target, 'GetSystemTime')
    end

    after_each(function()
        SetTimeOffset(0)
    end)

    it('maps fixed earth times to Vanadiel calendar bindings', function()
        local cases =
        {
            {
                name          = '2002-01-01T00:00:00Z',
                earth         = 1009843200,
                uniqueDay     = 9,
                year          = 0,
                month         = 1,
                monthDay      = 10,
                yearDay       = 9,
                weekday       = xi.day.EARTHSDAY,
                hour          = 9,
                minute        = 0,
                totd          = xi.time.DAY,
                moonPhase     = 12,
                moonDirection = 2,
                rseRace       = 2,
                rseLocation   = 1,
            },
            {
                name          = '2010-06-15T12:34:56Z',
                earth         = 1276605296,
                uniqueDay     = 77197,
                year          = 214,
                month         = 6,
                monthDay      = 8,
                yearDay       = 157,
                weekday       = xi.day.LIGHTNINGDAY,
                hour          = 11,
                minute        = 33,
                totd          = xi.time.DAY,
                moonPhase     = 7,
                moonDirection = 1,
                rseRace       = 2,
                rseLocation   = 1,
            },
            {
                name          = '2026-07-03T00:00:00Z',
                earth         = 1783036800,
                uniqueDay     = 223734,
                year          = 621,
                month         = 6,
                monthDay      = 25,
                yearDay       = 174,
                weekday       = xi.day.LIGHTSDAY,
                hour          = 9,
                minute        = 0,
                totd          = xi.time.DAY,
                moonPhase     = 90,
                moonDirection = 2,
                rseRace       = 7,
                rseLocation   = 0,
            },
        }

        for _, case in ipairs(cases) do
            setEarthTime(case.earth)

            assertEqual(VanadielTime(), case.earth - vanaEpoch, case.name .. ' VanadielTime')
            assertEqual(VanadielUniqueDay(), case.uniqueDay, case.name .. ' VanadielUniqueDay')
            assertEqual(VanadielYear(), case.year, case.name .. ' VanadielYear')
            assertEqual(VanadielMonth(), case.month, case.name .. ' VanadielMonth')
            assertEqual(VanadielDayOfTheMonth(), case.monthDay, case.name .. ' VanadielDayOfTheMonth')
            assertEqual(VanadielDayOfTheYear(), case.yearDay, case.name .. ' VanadielDayOfTheYear')
            assertEqual(VanadielDayOfTheWeek(), case.weekday, case.name .. ' VanadielDayOfTheWeek')
            assertEqual(VanadielHour(), case.hour, case.name .. ' VanadielHour')
            assertEqual(VanadielMinute(), case.minute, case.name .. ' VanadielMinute')
            assertEqual(VanadielTOTD(), case.totd, case.name .. ' VanadielTOTD')
            assertEqual(VanadielMoonPhase(), case.moonPhase, case.name .. ' VanadielMoonPhase')
            assertEqual(VanadielMoonDirection(), case.moonDirection, case.name .. ' VanadielMoonDirection')
            assertEqual(VanadielRSERace(), case.rseRace, case.name .. ' VanadielRSERace')
            assertEqual(VanadielRSELocation(), case.rseLocation, case.name .. ' VanadielRSELocation')
        end
    end)

    it('maps hour ranges to time of day buckets', function()
        local expected =
        {
            [0]  = xi.time.MIDNIGHT,
            [1]  = xi.time.MIDNIGHT,
            [2]  = xi.time.MIDNIGHT,
            [3]  = xi.time.MIDNIGHT,
            [4]  = xi.time.NEW_DAY,
            [5]  = xi.time.NEW_DAY,
            [6]  = xi.time.DAWN,
            [7]  = xi.time.DAY,
            [17] = xi.time.DUSK,
            [18] = xi.time.EVENING,
            [20] = xi.time.NIGHT,
        }

        for hour, totd in pairs(expected) do
            xi.test.world:setVanaTime(hour, 0)
            assertEqual(VanadielHour(), hour, 'VanadielHour')
            assertEqual(VanadielMinute(), 0, 'VanadielMinute')
            assertEqual(VanadielTOTD(), totd, 'VanadielTOTD')
        end
    end)
end)
