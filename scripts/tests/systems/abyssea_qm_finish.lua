require('scripts/globals/abyssea')

describe('Abyssea QM finish', function()
    it('ignores a non-confirming event finish', function()
        local p = { getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end }
        assert(xi.abyssea.qmOnEventFinish(p, 1020, 0, {}) == nil)
        assert(xi.abyssea.qmOnEventFinish(p, 1021, 1, {}) == nil)
    end)
end)
