-----------------------------------
-- Pure system tests for getVanaMidnight arithmetic and xi.vanaTime constants.
-----------------------------------

describe('Vana midnight pure plans', function()
    it('xi.vanaTime.DAY is 3456 earth seconds', function()
        assert(xi.vanaTime.DAY == 3456)
        assert(xi.vanaTime.HOUR == 144)
        assert(xi.vanaTime.WEEK == 8 * xi.vanaTime.DAY)
    end)

    local function secondsUntil(vanaTime, dayOffset)
        local secondsToMidnight = xi.vanaTime.DAY - (vanaTime % xi.vanaTime.DAY)
        if dayOffset ~= nil and dayOffset ~= 0 then
            secondsToMidnight = secondsToMidnight + (dayOffset * xi.vanaTime.DAY)
        end

        return secondsToMidnight
    end

    it('seconds until midnight from remainder', function()
        local day = xi.vanaTime.DAY
        assert(secondsUntil(0, 0) == day)
        assert(secondsUntil(day, 0) == day)
        assert(secondsUntil(day / 2, 0) == day / 2)
        assert(secondsUntil(day - 1, 0) == 1)
        assert(secondsUntil(day / 2, 2) == day / 2 + 2 * day)
    end)

    it('system midnight is systemTime + wait', function()
        local sys  = 1000000
        local vana = xi.vanaTime.DAY / 4
        local wait = secondsUntil(vana, 0)
        assert(sys + wait == sys + (xi.vanaTime.DAY - vana))
    end)
end)
