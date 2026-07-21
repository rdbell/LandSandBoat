-----------------------------------
-- Pure system tests for ffxiRotToDegrees / lateralTranslate / getNearPosition.
-----------------------------------

describe('Spatial rotation translate pure plans', function()
    it('ffxiRotToDegrees uses 360/255', function()
        assert(utils.ffxiRotToDegrees(0) == 0)
        assert(utils.ffxiRotToDegrees(255) == 360)
        assert(math.abs(utils.ffxiRotToDegrees(128) - (360 * 128 / 255)) < 1e-9)
    end)

    it('lateralTranslateWithOriginRotation at rot 0', function()
        local origin = { x = 10, y = 5, z = 20, rot = 0 }
        local got    = utils.lateralTranslateWithOriginRotation(origin, { x = 3, z = 4 })
        assert(got.x == 13)
        assert(got.y == 5)
        assert(got.z == 24)
        assert(got.rot == 0)
    end)

    it('lateralTranslateWithOriginRotation rotates local +X at rot 64', function()
        local origin = { x = 0, y = 0, z = 0, rot = 64 }
        local got    = utils.lateralTranslateWithOriginRotation(origin, { x = 1, z = 0 })
        local degrees = utils.ffxiRotToDegrees(64)
        local rads    = math.rad(degrees)
        local wantX   = math.cos(rads) * 1
        local wantZ   = -math.sin(rads) * 1
        assert(math.abs(got.x - wantX) < 1e-6)
        assert(math.abs(got.z - wantZ) < 1e-6)
        assert(got.rot == 64)
    end)

    it('getNearPosition absolute offset', function()
        local origin = { x = 10, y = 2, z = -3 }
        local got    = utils.getNearPosition(origin, 2.5, 0)
        assert(math.abs(got.x - 12.5) < 1e-6)
        assert(got.y == 2)
        assert(math.abs(got.z - (-3)) < 1e-6)

        got = utils.getNearPosition(origin, 2.5, math.pi)
        assert(math.abs(got.x - 7.5) < 1e-6)
    end)
end)
