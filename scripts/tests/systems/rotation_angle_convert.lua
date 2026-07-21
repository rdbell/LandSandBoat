-----------------------------------
-- Pure system tests for utils.rotationToAngle / angleToRotation /
-- angleWithin / distanceWithin.
-----------------------------------

describe('rotation/angle convert pure plans', function()
    it('rotationToAngle uses 2pi/256 factor', function()
        assert(utils.rotationToAngle(0) == 0)
        local half = utils.rotationToAngle(128)
        assert(math.abs(half - math.pi) < 1e-9)
        local full = utils.rotationToAngle(256)
        assert(math.abs(full - 2 * math.pi) < 1e-9)
    end)

    it('angleToRotation is inverse of rotationToAngle', function()
        for _, r in ipairs({ 0, 1, 64, 128, 200, 255 }) do
            local back = utils.angleToRotation(utils.rotationToAngle(r))
            assert(math.abs(back - r) < 1e-6)
        end
    end)

    it('getWorldRotation equals angleToRotation(getWorldAngle)', function()
        local A = { x = 0, y = 0, z = 0 }
        local B = { x = 1, y = 0, z = 0 }
        local ang = utils.getWorldAngle(A, B)
        local rot = utils.getWorldRotation(A, B)
        assert(math.abs(rot - utils.angleToRotation(ang)) < 1e-9)
    end)

    it('angleWithin uses world-angle difference', function()
        local origin = { x = 0, y = 0, z = 0 }
        local near1 = { x = 1, y = 0, z = 0 }
        local near2 = { x = 1, y = 0, z = 0.01 }
        local far = { x = -1, y = 0, z = 0 }
        assert(utils.angleWithin(origin, near1, near2, 0.5) == true)
        assert(utils.angleWithin(origin, near1, far, 0.5) == false)
    end)

    it('distanceWithin uses squared distance', function()
        local A = { x = 0, y = 0, z = 0 }
        -- 3-4-5 on XZ so ignoreVertical keeps the 4
        local B = { x = 3, y = 0, z = 4 } -- dist 5
        assert(utils.distanceWithin(A, B, 5, true) == true)
        assert(utils.distanceWithin(A, B, 4.9, true) == false)
        local C = { x = 0, y = 100, z = 0 }
        assert(utils.distanceWithin(A, C, 0.1, true) == true)
        assert(utils.distanceWithin(A, C, 0.1, false) == false)
    end)
end)
