-----------------------------------
-- Pure system tests for xi.data.augments.bundles catalog.
-----------------------------------

describe('augment bundles pure catalog', function()
    it('bundle 1 has double damage and store TP mods', function()
        local b = xi.data.augments.bundles[1]
        assert(#b == 2)
        assert(b[1][1][1] == 1296)
        assert(b[1][2][1] == 0)  -- rank 0 (1-based index 1)
        assert(b[1][2][6] == 10) -- rank 5
        assert(b[2][1][1] == 142)
    end)

    it('bundle 12 includes automaton parametric effect', function()
        local b = xi.data.augments.bundles[12]
        local found = false
        for _, eff in ipairs(b) do
            if eff[1][1] == xi.augment.parametric.AUTOMATON then
                found = true
                assert(eff[1][2] == 1314)
                assert(eff[2][6] == 5) -- rank 5
            end
        end
        assert(found)
    end)

    it('catalog spans 1..496', function()
        assert(xi.data.augments.bundles[1] ~= nil)
        assert(xi.data.augments.bundles[496] ~= nil)
        local n = 0
        for _ in pairs(xi.data.augments.bundles) do
            n = n + 1
        end
        assert(n == 496)
    end)
end)
