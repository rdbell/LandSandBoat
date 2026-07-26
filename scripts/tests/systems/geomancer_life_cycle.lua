require('scripts/globals/job_utils/geomancer')

describe('Geomancer Life Cycle host', function()
    it('routes to the luopan, transfers HP, drains the player, and returns the transfer', function()
        local restored
        local drained
        local routed
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            restoreHP = function(_, amount) restored = amount end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
            getHP = function() return 1000 end,
            getMod = function(_, mod)
                assert(mod == xi.mod.LIFE_CYCLE_EFFECT)
                return 15
            end,
            delHP = function(_, amount) drained = amount end,
        }
        local action = {
            ID = function(_, sourceID, targetID) routed = { sourceID, targetID } end,
        }

        assert(xi.job_utils.geomancer.lifeCycle(player, {}, {}, action) == 375)
        assert(routed[1] == 7 and routed[2] == 42)
        assert(restored == 375 and drained == 250)
    end)
end)
