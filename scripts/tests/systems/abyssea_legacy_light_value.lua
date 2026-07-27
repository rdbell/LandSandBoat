require('scripts/globals/abyssea')

describe('Abyssea legacy light value', function()
    it('extracts an overlapping byte after the light-index shift', function()
        local player = { getCharVar = function() return 0xD2B6 end }
        assert(xi.abyssea.getLightValue(player, 1) == 0xB6)
        assert(xi.abyssea.getLightValue(player, 2) == 0xAD)
        assert(xi.abyssea.getLightValue(player, 3) == 0x2B)
    end)
end)
