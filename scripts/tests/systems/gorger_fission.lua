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

describe('Fission mob skill', function()
    it('calls its available add IDs with fixed pet parameters and no skill message', function()
        local fission = require('scripts/actions/mobskills/fission')
        local originalCallPets = xi.mob.callPets
        local calls = {}
        local message = nil

        xi.mob.callPets = function(mob, pets, params)
            calls.mob = mob
            calls.pets = pets
            calls.params = params
        end

        local mob = {
            getID = function() return 100 end,
            getLocalVar = function(_, key)
                return key == 'fissionAdds' and 3 or 1
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(fission.onMobSkillCheck(nil, mob, skill) == 0)
        assert(fission.onMobWeaponSkill(mob, nil, skill, nil) == 0)

        xi.mob.callPets = originalCallPets

        assert(calls.mob == mob)
        assert(calls.pets[1] == 102 and calls.pets[2] == 103 and calls.pets[3] == 104)
        assert(calls.params.maxSpawns == 1 and calls.params.noAnimation)
        assert(calls.params.dieWithOwner and calls.params.superlink and calls.params.ignoreBusy)
        assert(message == xi.msg.basic.NONE)
    end)

    it('calls no pets when there are no remaining adds', function()
        local fission = require('scripts/actions/mobskills/fission')
        local originalCallPets = xi.mob.callPets
        local pets = nil

        xi.mob.callPets = function(_, ids) pets = ids end

        local mob = {
            getID = function() return 100 end,
            getLocalVar = function() return 0 end,
        }
        local skill = { setMsg = function() end }
        fission.onMobWeaponSkill(mob, nil, skill, nil)

        xi.mob.callPets = originalCallPets

        assert(#pets == 0)
    end)
end)
