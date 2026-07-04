describe('Earth Time', function()
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

    it('maps fixed earth times to JST calendar bindings and reset timestamps', function()
        local cases =
        {
            {
                name          = '2026-07-03T00:00:00Z',
                earth         = 1783036800,
                jstYear       = 2026,
                jstMonth      = 7,
                jstMonthDay   = 3,
                jstYearDay    = 183,
                jstWeekday    = 5,
                jstHour       = 9,
                jstMidnight   = 1783090800,
                nextJstWeek   = 1783263600,
            },
            {
                name          = '2026-07-05T14:59:59Z',
                earth         = 1783263599,
                jstYear       = 2026,
                jstMonth      = 7,
                jstMonthDay   = 5,
                jstYearDay    = 186,
                jstWeekday    = 0,
                jstHour       = 23,
                jstMidnight   = 1783263600,
                nextJstWeek   = 1783263600,
            },
            {
                name          = '2026-07-05T15:00:00Z',
                earth         = 1783263600,
                jstYear       = 2026,
                jstMonth      = 7,
                jstMonthDay   = 6,
                jstYearDay    = 186,
                jstWeekday    = 1,
                jstHour       = 0,
                jstMidnight   = 1783263600,
                nextJstWeek   = 1783782000,
            },
            {
                name          = '2001-12-31T15:00:00Z',
                earth         = 1009810800,
                jstYear       = 2002,
                jstMonth      = 1,
                jstMonthDay   = 1,
                jstYearDay    = 0,
                jstWeekday    = 2,
                jstHour       = 0,
                jstMidnight   = 1009810800,
                nextJstWeek   = 1010242800,
            },
        }

        for _, case in ipairs(cases) do
            setEarthTime(case.earth)

            assertEqual(GetSystemTime(), case.earth, case.name .. ' GetSystemTime')
            assertEqual(JstYear(), case.jstYear, case.name .. ' JstYear')
            assertEqual(JstMonth(), case.jstMonth, case.name .. ' JstMonth')
            assertEqual(JstDayOfTheMonth(), case.jstMonthDay, case.name .. ' JstDayOfTheMonth')
            assertEqual(JstDayOfTheYear(), case.jstYearDay, case.name .. ' JstDayOfTheYear')
            assertEqual(JstDayOfTheWeek(), case.jstWeekday, case.name .. ' JstDayOfTheWeek')
            assertEqual(JstHour(), case.jstHour, case.name .. ' JstHour')
            assertEqual(JstMidnight(), case.jstMidnight, case.name .. ' JstMidnight')
            assertEqual(NextJstDay(), case.jstMidnight, case.name .. ' NextJstDay')
            assertEqual(NextJstWeek(), case.nextJstWeek, case.name .. ' NextJstWeek')
            assertEqual(NextConquestTally(), case.nextJstWeek, case.name .. ' NextConquestTally')
        end
    end)
end)
