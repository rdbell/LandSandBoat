-----------------------------------
-- Pure system tests for utils.range (numeric) / contains / hasKey.
-----------------------------------

describe('Utils range and contains pure plans', function()
    it('range numeric with default and custom step', function()
        local t = utils.range(1, 3)
        assert(#t == 3 and t[1] == 1 and t[2] == 2 and t[3] == 3)
        t = utils.range(1, 5, 2)
        assert(#t == 3 and t[1] == 1 and t[2] == 3 and t[3] == 5)
        t = utils.range(5, 1, -1)
        assert(#t == 5 and t[1] == 5 and t[5] == 1)
        t = utils.range(3, 3)
        assert(#t == 1 and t[1] == 3)
    end)

    it('contains finds value in list', function()
        assert(utils.contains(2, { 1, 2, 3 }))
        assert(not utils.contains(4, { 1, 2, 3 }))
    end)

    it('hasKey finds key in map-like table', function()
        local t = { [10] = 'a', [20] = 'b' }
        assert(utils.hasKey(10, t))
        assert(not utils.hasKey(30, t))
        local s = { x = 1 }
        assert(utils.hasKey('x', s))
        assert(not utils.hasKey('y', s))
    end)
end)
