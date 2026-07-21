-----------------------------------
-- Pure system tests for xi.data.augments.rpCurves.
-----------------------------------

describe('augment rpCurves pure catalog', function()
    it('curve A rank corners', function()
        local a = xi.data.augments.rpCurves[xi.augment.rpCurve.A]
        assert(a[0] == 30)
        assert(a[5] == 220)
        assert(a[30] == 4750)
    end)

    it('curve B rank corners', function()
        local b = xi.data.augments.rpCurves[xi.augment.rpCurve.B]
        assert(b[0] == 750)
        assert(b[9] == 10000)
        assert(b[30] == 55800)
    end)

    it('mezzotint duplicates curve A', function()
        local a = xi.data.augments.rpCurves[xi.augment.rpCurve.A]
        local m = xi.data.augments.rpCurves[xi.augment.rpCurve.MEZZOTINT]
        for rank = 0, 14 do
            assert(m[rank] == a[rank])
        end
    end)
end)
