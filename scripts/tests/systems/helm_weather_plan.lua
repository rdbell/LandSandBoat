-----------------------------------
-- Pure system tests for HELM weatherChange pure plan (slice 6159).
-----------------------------------

describe('helm weather pure plan', function()
    local NORMAL    = 0
    local DISAPPEAR = 2

    local function contains(list, v)
        for _, x in ipairs(list) do
            if x == v then return true end
        end
        return false
    end

    local function weatherStatus(currentWeather, neededWeather)
        if contains(neededWeather, currentWeather) then
            return NORMAL
        end
        return DISAPPEAR
    end

    -- Plan which points need a status change: only when current status != desired
    local function planChanges(currentWeather, neededWeather, points)
        -- points: { { id = n, status = s }, ... }
        local desired = weatherStatus(currentWeather, neededWeather)
        local changes = {}
        for _, p in ipairs(points) do
            if p.status ~= desired then
                changes[#changes + 1] = { id = p.id, status = desired }
            end
        end
        return desired, changes
    end

    it('status normal when weather matches needed', function()
        assert(weatherStatus(4, { 4, 5 }) == NORMAL)
        assert(weatherStatus(1, { 4, 5 }) == DISAPPEAR)
        assert(weatherStatus(5, {}) == DISAPPEAR)
    end)

    it('only changes points with mismatched status', function()
        local desired, changes = planChanges(4, { 4 }, {
            { id = 10, status = DISAPPEAR },
            { id = 11, status = NORMAL },
            { id = 12, status = DISAPPEAR },
        })
        assert(desired == NORMAL)
        assert(#changes == 2 and changes[1].id == 10 and changes[2].id == 12)
        assert(changes[1].status == NORMAL)
    end)

    it('no changes when already matching', function()
        local _, changes = planChanges(1, { 2 }, {
            { id = 1, status = DISAPPEAR },
            { id = 2, status = DISAPPEAR },
        })
        assert(#changes == 0)
    end)
end)
