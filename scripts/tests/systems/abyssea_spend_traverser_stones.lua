require('scripts/globals/abyssea')

describe('Abyssea spend traverser stones', function()
    it('removes held stones from six down to one and stops at the request', function()
        local held = { [1271] = true, [1273] = true, [1276] = true }
        local removed = {}
        local p = {
            hasKeyItem = function(_, ki) return held[ki] end,
            delKeyItem = function(_, ki) held[ki] = nil table.insert(removed, ki) end,
        }
        xi.abyssea.spendTravStones(p, 2)
        assert(removed[1] == 1276 and removed[2] == 1273 and held[1271])
    end)
end)
