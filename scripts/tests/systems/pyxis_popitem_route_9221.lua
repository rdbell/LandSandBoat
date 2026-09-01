require('scripts/globals/abyssea/sturdypyxis/popitem')

describe('Pyxis pop-item option routing (slice 9221)', function()
    it('routes upper-bit selectors one through eight to direct slots', function()
        for selector = 1, 8 do
            assert(xi.pyxis.popitem.routeSelection(selector * 0x10000 + 37) == selector)
        end
    end)

    it('routes selector nine to ordered treasure transfer', function()
        assert(xi.pyxis.popitem.routeSelection(9 * 0x10000 + 37) == 9)
    end)

    it('ignores every other selector', function()
        assert(xi.pyxis.popitem.routeSelection(0) == 0)
        assert(xi.pyxis.popitem.routeSelection(10 * 0x10000) == 0)
        assert(xi.pyxis.popitem.routeSelection(0xFFFFFFFF) == 0)
    end)
end)
