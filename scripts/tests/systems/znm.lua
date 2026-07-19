require('scripts/globals/znm')

describe('ZNM Sanraku trade rotation', function()
    it('rotates after 500 completed trades', function()
        assert(not xi.znm.shouldRotateSanrakuTrades(499))
        assert(xi.znm.shouldRotateSanrakuTrades(500))
        assert(xi.znm.shouldRotateSanrakuTrades(501))
    end)
end)
