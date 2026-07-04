describe('Next Game Time', function()
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

    it('rounds Vana timestamps up to the next requested interval', function()
        local cases =
        {
            {
                name         = 'epoch',
                earth        = 1009810800,
                nextHour     = 144,
                nextDay      = 3456,
                nextWeek     = 27648,
                nextCustom60 = 60,
            },
            {
                name         = '2002-01-01T00:00:00Z',
                earth        = 1009843200,
                nextHour     = 32544,
                nextDay      = 34560,
                nextWeek     = 55296,
                nextCustom60 = 32460,
            },
            {
                name         = '2026-07-03T00:00:00Z',
                earth        = 1783036800,
                nextHour     = 773226144,
                nextDay      = 773228160,
                nextWeek     = 773231616,
                nextCustom60 = 773226060,
            },
            {
                name         = 'boundary',
                earth        = 1009814256,
                nextHour     = 3600,
                nextDay      = 6912,
                nextWeek     = 27648,
                nextCustom60 = 3480,
            },
            {
                name         = 'pre epoch',
                earth        = 1009810799,
                nextHour     = 32,
                nextDay      = 896,
                nextWeek     = 11264,
                nextCustom60 = 44,
            },
        }

        for _, case in ipairs(cases) do
            setEarthTime(case.earth)

            assertEqual(GetSystemTime(), case.earth, case.name .. ' GetSystemTime')
            assertEqual(NextGameTime(xi.vanaTime.HOUR), case.nextHour, case.name .. ' hour')
            assertEqual(NextGameTime(xi.vanaTime.DAY), case.nextDay, case.name .. ' day')
            assertEqual(NextGameTime(xi.vanaTime.WEEK), case.nextWeek, case.name .. ' week')
            assertEqual(NextGameTime(60), case.nextCustom60, case.name .. ' 60 seconds')
        end
    end)
end)
