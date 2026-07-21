-----------------------------------
-- Pure system tests for scripts/utils/time_utils.lua.
-----------------------------------

describe('Time utils pure plans', function()
    it('minutes hours days convert to system seconds', function()
        assert(utils.minutes(3) == 180)
        assert(utils.hours(2) == 7200)
        assert(utils.days(1) == 86400)
        assert(utils.minutes(0) == 0)
    end)

    it('timeIsAfterOrEqual lexicographic year month day hour', function()
        local a = { year = 2020, month = 6, day = 15, hour = 12 }
        assert(utils.timeIsAfterOrEqual(a, a))
        assert(utils.timeIsAfterOrEqual({ year = 2020, month = 6, day = 15, hour = 13 }, a))
        assert(not utils.timeIsAfterOrEqual({ year = 2020, month = 6, day = 15, hour = 11 }, a))
        assert(utils.timeIsAfterOrEqual({ year = 2020, month = 6, day = 16, hour = 0 }, a))
        assert(not utils.timeIsAfterOrEqual({ year = 2020, month = 5, day = 20, hour = 23 }, a))
        assert(utils.timeIsAfterOrEqual({ year = 2021, month = 1, day = 1, hour = 0 }, a))
    end)

    it('timeIsBefore is exclusive of equal and mirrors earlier', function()
        local a = { year = 2020, month = 6, day = 15, hour = 12 }
        assert(not utils.timeIsBefore(a, a))
        assert(utils.timeIsBefore({ year = 2020, month = 6, day = 15, hour = 11 }, a))
        assert(not utils.timeIsBefore({ year = 2020, month = 6, day = 15, hour = 13 }, a))
        assert(utils.timeIsBefore({ year = 2019, month = 12, day = 31, hour = 23 }, a))
    end)

    it('vanadielClockTime pure inject form hour*100+minute', function()
        -- Live vanadielClockTime needs world time; pin the product shape.
        local function clock(hour, minute)
            return tonumber(hour .. string.format('%02d', minute))
        end

        assert(clock(4, 30) == 430)
        assert(clock(21, 30) == 2130)
        assert(clock(0, 5) == 5)
        assert(clock(0, 0) == 0)
        assert(clock(23, 59) == 2359)
    end)

    it('timeStringToMinutes parses HH:MM and rejects invalid', function()
        assert(utils.timeStringToMinutes('00:00') == 0)
        assert(utils.timeStringToMinutes('0:00') == 0)
        assert(utils.timeStringToMinutes('04:30') == 4 * 60 + 30)
        assert(utils.timeStringToMinutes('21:30') == 21 * 60 + 30)
        assert(utils.timeStringToMinutes('24:00') == 1440)
        assert(utils.timeStringToMinutes('23:59') == 23 * 60 + 59)
        assert(utils.timeStringToMinutes('9:05') == 9 * 60 + 5)
        assert(utils.timeStringToMinutes('25:00') == -1)
        assert(utils.timeStringToMinutes('24:01') == -1)
        assert(utils.timeStringToMinutes('12:60') == -1)
        assert(utils.timeStringToMinutes('12:5') == -1)
        assert(utils.timeStringToMinutes('') == -1)
        assert(utils.timeStringToMinutes('ab:cd') == -1)
    end)
end)
