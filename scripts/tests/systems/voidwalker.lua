require('scripts/globals/voidwalker')

describe('Voidwalker nearest mob', function()
    it('returns the closest candidate with its key item and distance', function()
        local candidates =
        {
            { mobId = 100, keyItem = 200 },
            { mobId = 101, keyItem = 201 },
            { mobId = 102, keyItem = 202 }
        }
        local distances = { [100] = 20, [101] = 5, [102] = 10 }
        local nearest = xi.voidwalker.nearestMob(candidates, function(mobId) return distances[mobId] end)

        assert(nearest.mobId == 101 and nearest.keyItem == 201 and nearest.distance == 5)
        assert(xi.voidwalker.nearestMob({}, function() return 1 end) == nil)
        assert(xi.voidwalker.nearestMob(nil, function() return 1 end) == nil)
        assert(xi.voidwalker.nearestMob(candidates, nil) == nil)
    end)
end)

describe('Voidwalker direction', function()
    it('classifies cardinal and diagonal offsets', function()
        assert(xi.voidwalker.direction(1, 0) == 0)
        assert(xi.voidwalker.direction(0, -1) == 2)
        assert(xi.voidwalker.direction(-1, 0) == 4)
        assert(xi.voidwalker.direction(0, 1) == 6)

        assert(xi.voidwalker.direction(1, -1) == 1)
        assert(xi.voidwalker.direction(-1, -1) == 3)
        assert(xi.voidwalker.direction(-1, 1) == 5)
        assert(xi.voidwalker.direction(1, 1) == 7)
    end)

    it('uses 20 and 70 degrees as the horizontal and vertical cutoffs', function()
        assert(xi.voidwalker.direction(1, math.tan(math.rad(19))) == 0)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(20))) == 0)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(21))) == 7)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(69))) == 7)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(70))) == 6)
        assert(xi.voidwalker.direction(1, math.tan(math.rad(71))) == 6)
    end)

    it('retains the northeast fallback for coincident positions', function()
        assert(xi.voidwalker.direction(0, 0) == 7)
    end)
end)
