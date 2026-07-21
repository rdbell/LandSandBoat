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
end)
