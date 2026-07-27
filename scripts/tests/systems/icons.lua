require('scripts/globals/icons')

describe('Name icon catalog', function()
    it('maps representative icons to their exact single-byte codes', function()
        assert(xi.icon.SQUARE == string.char(0x81))
        assert(xi.icon.SWORD_AND_SHIELD == string.char(0xA9))
        assert(xi.icon.INFORMATION_ICON == string.char(0xB1))
    end)
end)
