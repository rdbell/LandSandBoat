-----------------------------------
-- Pure system tests for sameSideOfLine / getAngleDifference / angleWithin /
-- getWorldAngle (scripts/utils/utils.lua).
-----------------------------------

describe('Spatial line and angle pure plans', function()
    it('getAngleDifference wraps across pi', function()
        assert(utils.getAngleDifference(0, 0) == 0)
        assert(math.abs(utils.getAngleDifference(0, math.pi / 2) - math.pi / 2) < 1e-12)
        assert(math.abs(utils.getAngleDifference(0, 3 * math.pi / 2) - math.pi / 2) < 1e-12)
    end)

    it('angleWithin uses difference threshold', function()
        local origin = { x = 0, y = 0, z = 0 }
        local a      = { x = 1, y = 0, z = 0 }
        local bNear  = { x = 1, y = 0, z = 0.01 }
        local bFar   = { x = -1, y = 0, z = 0 }
        assert(utils.angleWithin(origin, a, bNear, 0.5))
        assert(not utils.angleWithin(origin, a, bFar, 0.1))
    end)

    it('sameSideOfLine vertical and horizontal', function()
        local vline = { { 0, 0 }, { 0, 10 } } -- x=0
        local left1  = { x = -1, z = 5 }
        local left2  = { x = -2, z = 8 }
        local right1 = { x = 1, z = 5 }
        assert(utils.sameSideOfLine(vline, left1, left2))
        assert(not utils.sameSideOfLine(vline, left1, right1))

        local hline = { { 0, 0 }, { 10, 0 } } -- z=0
        local above = { x = 5, z = 1 }
        local below = { x = 5, z = -1 }
        assert(not utils.sameSideOfLine(hline, above, below))
        assert(utils.sameSideOfLine(hline, above, { x = 3, z = 2 }))
    end)

    it('getWorldAngle returns finite [0, 2pi] in each quadrant', function()
        local o = { x = 0, y = 0, z = 0 }
        for _, p in ipairs({
            { x = 1, z = 1 },
            { x = -1, z = 1 },
            { x = -1, z = -1 },
            { x = 1, z = -1 },
        }) do
            local a = utils.getWorldAngle(o, p)
            assert(a >= 0 and a <= 2 * math.pi + 1e-9, 'out of range ' .. tostring(a))
        end

        local rot = utils.getWorldRotation(o, { x = 1, z = 0 })
        assert(type(rot) == 'number' and rot == rot) -- not nan
    end)
end)
