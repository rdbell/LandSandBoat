require('scripts/globals/abyssea')

describe('Abyssea QM trade', function()
    local function trade(count, held)
        return { getItemCount = function() return count end, hasItemQty = function(_, item) return held[item] end }
    end

    it('rejects empty, mismatched, or incomplete required trades', function()
        assert(not xi.abyssea.qmOnTrade({}, {}, trade(1, { [1] = true }), 1, {}))
        assert(not xi.abyssea.qmOnTrade({}, {}, trade(1, { [1] = true }), 1, { 1, 2 }))
        assert(not xi.abyssea.qmOnTrade({}, {}, trade(2, { [1] = true }), 1, { 1, 2 }))
    end)
end)
