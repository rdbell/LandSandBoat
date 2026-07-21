-----------------------------------
-- Pure system tests for utils.any / all / sum / counter / filterArray / map.
-----------------------------------

describe('utils functional pure plans', function()
    it('any returns true when predicate matches', function()
        assert(utils.any({ 'a', 'b', 'c' }, function(k, v) return v == 'b' end) == true)
        assert(utils.any({ 'a', 'b' }, function(k, v) return v == 'z' end) == false)
        assert(utils.any({}, function(k, v) return true end) == false)
    end)

    it('all requires every element to pass', function()
        assert(utils.all({ 2, 4, 6 }, function(k, v) return v % 2 == 0 end) == true)
        assert(utils.all({ 2, 3, 6 }, function(k, v) return v % 2 == 0 end) == false)
        assert(utils.all({}, function(k, v) return false end) == true)
    end)

    it('sum applies mapper; counter counts matches', function()
        assert(utils.sum({ 1, 2, 3 }, function(k, v) return v end) == 6)
        local n = utils.sum({ 'a', 'a', 'b' }, utils.counter(function(k, v) return v == 'a' end))
        assert(n == 2)
    end)

    it('filterArray returns dense values that pass', function()
        local out = utils.filterArray({ 'a', 'b', 'c', 'd' }, function(k, v) return v >= 'c' end)
        assert(#out == 2 and out[1] == 'c' and out[2] == 'd')
    end)

    it('map transforms values keeping keys', function()
        local out = utils.map({ 1, 2, 3 }, function(k, v) return v * 2 end)
        assert(out[1] == 2 and out[2] == 4 and out[3] == 6)
    end)

    it('each visits all pairs', function()
        local keys, vals = {}, {}
        utils.each({ 'a', 'b', 'c' }, function(k, v)
            keys[#keys + 1] = k
            vals[#vals + 1] = v
        end)
        assert(#keys == 3 and #vals == 3)
        -- keys are 1-based array indices (order from pairs is undefined, so sort)
        table.sort(keys)
        table.sort(vals)
        assert(keys[1] == 1 and keys[2] == 2 and keys[3] == 3)
        assert(vals[1] == 'a' and vals[2] == 'b' and vals[3] == 'c')
    end)

    it('filter preserves original keys (sparse)', function()
        local out = utils.filter({ 'a', 'b', 'c', 'd' }, function(k, v) return v >= 'c' end)
        assert(out[3] == 'c' and out[4] == 'd')
        assert(out[1] == nil and out[2] == nil)
        local n = 0
        for _ in pairs(out) do
            n = n + 1
        end
        assert(n == 2)
    end)
end)
