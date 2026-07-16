require('scripts/mixins/families/gorger_nm')

describe('Gorger Fission gate', function()
    it('permits Fission only for an existing unspawned add and honors offsets', function()
        local original = _G.GetMobByID
        local parent = { getID = function() return 100 end, getLocalVar = function(_, key) return key == 'fissionAdds' and 2 or 1 end }
        local pets = { [102] = { isSpawned = function() return true end }, [103] = { isSpawned = function() return false end } }
        _G.GetMobByID = function(id) return pets[id] end
        assert(xi.mix.gorger.canUseFission(parent))
        pets[103] = { isSpawned = function() return true end }
        assert(not xi.mix.gorger.canUseFission(parent))
        _G.GetMobByID = original
    end)

    it('rejects zero adds and missing add lookups', function()
        local original = _G.GetMobByID
        _G.GetMobByID = function() return nil end
        assert(not xi.mix.gorger.canUseFission({ getID = function() return 1 end, getLocalVar = function(_, key) return key == 'fissionAdds' and 0 or 0 end }))
        assert(not xi.mix.gorger.canUseFission({ getID = function() return 1 end, getLocalVar = function(_, key) return key == 'fissionAdds' and 2 or 0 end }))
        _G.GetMobByID = original
    end)

    it('installs all SPAWN pool mappings', function()
        local callback
        local host = { addListener = function(_, event, _, fn) if event == 'SPAWN' then callback = fn end end }
        g_mixins.families.gorger_nm(host)
        local cases = { { xi.mobPool.HADAL_SATIATOR, 3, 0 }, { xi.mobPool.INGESTER, 4, 0 }, { xi.mobPool.INGURGITATOR, 2, 1 }, { xi.mobPool.PROCREATOR, 4, 0 }, { xi.mobPool.PROGENERATOR, 4, 0 }, { xi.mobPool.PROPAGATOR, 2, 0 } }
        for _, c in ipairs(cases) do
            local vars = {}
            callback({ getPool = function() return c[1] end, setLocalVar = function(_, key, value) vars[key] = value end })
            assert(vars.fissionAdds == c[2] and vars.fissionOffset == c[3])
        end
    end)
end)
