-----------------------------------
-- Pure system tests for utils.mask bit helpers.
-----------------------------------

describe('utils.mask pure plans', function()
    it('getBit reads pos-th bit', function()
        assert(utils.mask.getBit(0, 0) == false)
        assert(utils.mask.getBit(0x10, 4) == true)
        assert(utils.mask.getBit(0x10, 3) == false)
    end)

    it('setBit sets and clears', function()
        assert(utils.mask.setBit(0, 3, true) == 0x08)
        assert(utils.mask.setBit(0x08, 3, false) == 0)
        assert(utils.mask.setBit(0, 2, 1) == 0x04)
        assert(utils.mask.setBit(0x04, 2, 0) == 0)
    end)

    it('countBits counts within length', function()
        assert(utils.mask.countBits(0x0F, 8) == 4)
        assert(utils.mask.countBits(0xFF, 4) == 4)
        assert(utils.mask.countBits(0, 32) == 0)
        -- omitted len defaults to 32
        assert(utils.mask.countBits(0x0F) == 4)
    end)

    it('isFull checks all bits true in length', function()
        assert(utils.mask.isFull(0xFF, 8) == true)
        assert(utils.mask.isFull(0xFE, 8) == false)
        assert(utils.mask.isFull(0x07, 3) == true)
        assert(utils.mask.isFull(0x03, 3) == false)
    end)

    it('splitBits chunks little-endian sized fields', function()
        local bytes = utils.mask.splitBits(0x12345678, 8)
        assert(#bytes == 4)
        assert(bytes[1] == 0x78)
        assert(bytes[2] == 0x56)
        assert(bytes[3] == 0x34)
        assert(bytes[4] == 0x12)

        local words = utils.mask.splitBits(0x12345678, 16)
        assert(#words == 2)
        assert(words[1] == 0x5678)
        assert(words[2] == 0x1234)
    end)

    it('varSplit splits at splitBit (default 16)', function()
        local low, high = utils.mask.varSplit(0x12345678, 16)
        assert(low == 0x5678)
        assert(high == 0x1234)

        low, high = utils.mask.varSplit(0x12345678)
        assert(low == 0x5678)
        assert(high == 0x1234)

        low, high = utils.mask.varSplit(0xAB, 8)
        assert(low == 0xAB)
        assert(high == 0)
    end)
end)
