-----------------------------------
-- Pure system tests for utils.clamp.
-----------------------------------

describe('Utils clamp pure plans', function()
    it('clamps below min and above max', function()
        assert(utils.clamp(5, 0, 10) == 5)
        assert(utils.clamp(-1, 0, 10) == 0)
        assert(utils.clamp(11, 0, 10) == 10)
        assert(utils.clamp(0, 0, 0) == 0)
    end)

    it('returns input when min > max', function()
        assert(utils.clamp(5, 10, 0) == 5)
        assert(utils.clamp(0.5, 1, 0) == 0.5)
    end)

    it('works for floats', function()
        assert(utils.clamp(1.5, 0, 1) == 1)
        assert(utils.clamp(-0.5, 0, 1) == 0)
        assert(utils.clamp(0.25, 0, 1) == 0.25)
    end)
end)
