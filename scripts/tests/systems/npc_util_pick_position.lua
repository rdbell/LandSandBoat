-----------------------------------
-- Pure system tests for npcUtil.pickNewPosition / queueMove normalize (slice 6171).
-----------------------------------

describe('npcutil pickNewPosition pure plan', function()
    -- Match floor coords to find current index (1-based); default 1 if not found.
    local function findCurrentIndex(positions, x, y, z)
        for i, v in ipairs(positions) do
            if
                math.floor(v[1]) == math.floor(x) and
                math.floor(v[2]) == math.floor(y) and
                math.floor(v[3]) == math.floor(z)
            then
                return i
            end
        end
        return 1
    end

    -- pick inject is 1..tableSize; when !allowCurrent and pick==current, use altPick.
    local function planPickIndex(tableSize, currentIndex, allowCurrent, pick, altPick)
        if tableSize <= 0 then
            return nil
        end
        if allowCurrent then
            if pick < 1 then pick = 1 end
            if pick > tableSize then pick = tableSize end
            return pick
        end
        if tableSize == 1 then
            return 1 -- cannot avoid current when only one slot
        end
        if pick == currentIndex then
            pick = altPick
        end
        if pick < 1 then pick = 1 end
        if pick > tableSize then pick = tableSize end
        if pick == currentIndex then
            -- force different: wrap
            pick = (currentIndex % tableSize) + 1
        end
        return pick
    end

    -- queueMove point normalize: {x,y,z,rot?} → array form
    local function normalizePoint(point)
        if point.rot then
            return { point.x, point.y, point.z, point.rot }
        elseif point.x then
            return { point.x, point.y, point.z }
        end
        return point -- already array-like
    end

    it('finds current position by floored coords', function()
        local pos = {
            { 10.4, 1.2, -3.9 },
            { 20.0, 0.0, 0.0 },
            { -5.1, 2.0, 8.0 },
        }
        assert(findCurrentIndex(pos, 10.9, 1.8, -3.1) == 1)
        assert(findCurrentIndex(pos, 20.4, 0.1, 0.9) == 2)
        assert(findCurrentIndex(pos, 0, 0, 0) == 1) -- default
    end)

    it('allows current position when flag set', function()
        assert(planPickIndex(3, 2, true, 2, 1) == 2)
        assert(planPickIndex(3, 2, true, 1, 1) == 1)
    end)

    it('avoids current when not allowed', function()
        assert(planPickIndex(3, 2, false, 1, 1) == 1)
        assert(planPickIndex(3, 2, false, 2, 3) == 3)
        assert(planPickIndex(3, 2, false, 2, 2) == 3) -- wrap: (2%3)+1=3
    end)

    it('handles single-slot table', function()
        assert(planPickIndex(1, 1, false, 1, 1) == 1)
        assert(planPickIndex(0, 1, true, 1, 1) == nil)
    end)

    it('normalizes queueMove points', function()
        local a = normalizePoint({ x = 1, y = 2, z = 3, rot = 4 })
        assert(a[1] == 1 and a[2] == 2 and a[3] == 3 and a[4] == 4)
        local b = normalizePoint({ x = 5, y = 6, z = 7 })
        assert(b[1] == 5 and b[2] == 6 and b[3] == 7 and b[4] == nil)
        local c = normalizePoint({ 9, 8, 7, 6 })
        assert(c[1] == 9 and c[4] == 6)
    end)
end)
