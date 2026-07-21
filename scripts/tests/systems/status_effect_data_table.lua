-----------------------------------
-- Pure system tests for status effect dataTable density (slice 6076).
-----------------------------------

describe('status effect dataTable density', function()
    it('has 38 effect rows', function()
        local n = 0
        for id, row in pairs(xi.data.statusEffect.dataTable) do
            n = n + 1
            assert(type(id) == 'number')
            assert(type(row) == 'table')
            assert(#row >= 10 or row[1] ~= nil)
        end
        assert(n == 38, 'expected 38 rows, got ' .. tostring(n))
    end)

    it('pins SLEEP_I BIND POISON elements', function()
        local t = xi.data.statusEffect.dataTable
        assert(t[xi.effect.SLEEP_I] ~= nil)
        assert(t[xi.effect.BIND] ~= nil)
        assert(t[xi.effect.POISON] ~= nil)
        -- Associated element column (5): dark / ice / water
        assert(t[xi.effect.SLEEP_I][5] == xi.element.DARK or t[xi.effect.SLEEP_I][5] == xi.element.DARK)
        assert(t[xi.effect.BIND][5] == xi.element.ICE)
        assert(t[xi.effect.POISON][5] == xi.element.WATER)
    end)

    it('NONE and TERROR rows present', function()
        assert(xi.data.statusEffect.dataTable[xi.effect.NONE] ~= nil)
        assert(xi.data.statusEffect.dataTable[xi.effect.TERROR] ~= nil)
    end)
end)
