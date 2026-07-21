-----------------------------------
-- Pure system tests for RoE records full catalog density (slice 6073).
-----------------------------------

describe('roe records full catalog density', function()
    it('has 1078 top-level record keys', function()
        local n = 0
        for id, rec in pairs(xi.roe.records) do
            n = n + 1
            assert(type(id) == 'number')
            assert(type(rec) == 'table')
        end
        assert(n == 1078, 'expected 1078 records, got ' .. tostring(n))
    end)

    it('pins tutorial basics and hidden unlock', function()
        assert(xi.roe.records[1] ~= nil)
        assert(xi.roe.records[1].reward.sparks == 100)
        assert(xi.roe.records[1].reward.keyItem == xi.ki.MEMORANDOLL)
        assert(xi.roe.records[2].trigger == xi.roeTrigger.DEFEAT_MOB)
        assert(xi.roe.records[4085] ~= nil)
        assert(xi.roe.records[4085].flags['hidden'] == true)
    end)

    it('preserves explicit increment 0 on aggregate objectives', function()
        -- Lua treats 0 as truthy so initialize must not replace increment.
        assert(xi.roe.records[29].increment == 0)
        assert(xi.roe.records[29].goal == 100000)
        assert(xi.roe.records[29].trigger == xi.roeTrigger.DEAL_DAMAGE)
        assert(xi.roe.records[4013].increment == 0)
    end)

    it('includes previously sample-deferred unity and timed IDs', function()
        assert(xi.roe.records[3492] ~= nil)
        assert(xi.roe.records[4008] ~= nil)
        assert(xi.roe.records[4021] ~= nil)
        -- Known LSB gaps stay absent
        assert(xi.roe.records[3001] == nil)
        assert(xi.roe.records[784] == nil)
    end)
end)
