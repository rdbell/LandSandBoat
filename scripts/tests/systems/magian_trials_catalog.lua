-----------------------------------
-- Pure system tests for magian trials catalog density (slice 6071).
-----------------------------------

describe('magian trials full catalog density', function()
    it('has 759 top-level trial keys and no trial 1', function()
        local n = 0
        for id, _ in pairs(xi.magian.trials) do
            n = n + 1
            assert(type(id) == 'number')
            assert(id ~= 1)
        end
        assert(n == 759)
        assert(xi.magian.trials[1] == nil)
        assert(xi.magian.trials[2] ~= nil)
        assert(xi.magian.trials[2].numRequired == 3)
        assert(xi.magian.trials[2].requiredItem.itemId == xi.item.PEELER)
    end)

    it('opening and late ladder pins', function()
        assert(xi.magian.trials[5056] ~= nil)
        -- AERO family not relevant; pin a high ID present upstream
        local t = xi.magian.trials[2]
        assert(t.previousTrial == 0)
        assert(t.rewardItem.itemId == xi.item.RENEGADE)
    end)
end)
