require('scripts/globals/abyssea')

describe('Abyssea light mask pack/unpack', function()
    it('unpacks lights 1-4 from the first mask', function()
        -- bytes 0x04030201 -> 1,2,3,4
        local lights = xi.abyssea.unpackLights(0x04030201, 0)
        assert(lights[1] == 1 and lights[2] == 2 and lights[3] == 3 and lights[4] == 4)
    end)

    it('unpacks lights 5-7 from the second mask', function()
        local lights = xi.abyssea.unpackLights(0, 0x00070605)
        assert(lights[5] == 5 and lights[6] == 6 and lights[7] == 7)
    end)

    -- packLights shifts lights 5/6/7 by 32/40/48 rather than 0/8/16. It only
    -- agrees with unpackLights because the bit library reduces shift counts
    -- modulo 32. This pins that reliance explicitly.
    it('packs lights 5-7 back to byte offsets 0, 8, and 16', function()
        local first, second = xi.abyssea.packLights({ 0, 0, 0, 0, 5, 6, 7 })

        assert(first == 0)
        assert(second == 0x00070605, string.format('%08X', second))
    end)

    it('round-trips every light through pack and unpack', function()
        local original = { 11, 22, 33, 44, 55, 66, 77 }
        local first, second = xi.abyssea.packLights(original)
        local restored = xi.abyssea.unpackLights(first, second)

        for i = 1, 7 do
            assert(restored[i] == original[i], i .. ': ' .. tostring(restored[i]))
        end
    end)

    it('round-trips the maximum byte value in every slot', function()
        local original = { 255, 255, 255, 255, 255, 255, 255 }
        local first, second = xi.abyssea.packLights(original)
        local restored = xi.abyssea.unpackLights(first, second)

        for i = 1, 7 do
            assert(restored[i] == 255, i)
        end
    end)
end)
