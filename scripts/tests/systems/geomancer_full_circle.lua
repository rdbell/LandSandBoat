require('scripts/globals/job_utils/geomancer')

describe('Geomancer Full Circle host', function()
    it('restores Curative Recantation HP then MP and despawns the luopan', function()
        local calls = {}
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getHPP = function() return 50 end,
        }
        local player = {
            getPet = function() return luopan end,
            getLocalVar = function(_, key)
                assert(key == 'MP_COST')
                return 100
            end,
            getMerit = function(_, merit)
                return merit == xi.merit.FULL_CIRCLE_EFFECT and 5 or 1
            end,
            getMod = function(_, mod)
                return mod == xi.mod.FULL_CIRCLE and 0 or 0
            end,
            restoreHP = function(_, amount) table.insert(calls, { 'hp', amount }) end,
            restoreMP = function(_, amount) table.insert(calls, { 'mp', amount }) end,
            despawnPet = function() table.insert(calls, { 'despawn' }) end,
        }

        assert(xi.job_utils.geomancer.fullCircle(player, {}, {}) == nil)
        assert(#calls == 3 and calls[1][1] == 'hp' and calls[1][2] == 60)
        assert(calls[2][1] == 'mp' and calls[2][2] == 50 and calls[3][1] == 'despawn')
    end)
end)
