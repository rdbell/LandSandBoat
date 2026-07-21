-----------------------------------
-- Pure system tests for utils.intToBinary / toBytes / toWords / defaultIfNil.
-----------------------------------

describe('Utils binary and default pure plans', function()
    it('intToBinary builds digit string', function()
        assert(utils.intToBinary(0) == '0')
        assert(utils.intToBinary(1) == '1')
        assert(utils.intToBinary(2) == '10')
        assert(utils.intToBinary(5) == '101')
        assert(utils.intToBinary(255) == '11111111')
        assert(utils.intToBinary(10) == '1010')
    end)

    it('toBytes little-endian splits 0x12345678', function()
        local b0, b1, b2, b3 = utils.toBytes(0x12345678)
        assert(b0 == 0x78)
        assert(b1 == 0x56)
        assert(b2 == 0x34)
        assert(b3 == 0x12)
        b0, b1, b2, b3 = utils.toBytes(0)
        assert(b0 == 0 and b1 == 0 and b2 == 0 and b3 == 0)
    end)

    it('toWords little-endian splits 0x12345678', function()
        local w0, w1 = utils.toWords(0x12345678)
        assert(w0 == 0x5678)
        assert(w1 == 0x1234)
    end)

    it('defaultIfNil returns default only for nil', function()
        assert(utils.defaultIfNil(nil, 'fallback') == 'fallback')
        assert(utils.defaultIfNil('value', 'fallback') == 'value')
        assert(utils.defaultIfNil(0, 99) == 0) -- 0 is not nil
        assert(utils.defaultIfNil(false, true) == false)
    end)
end)
