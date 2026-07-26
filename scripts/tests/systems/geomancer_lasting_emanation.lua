require('scripts/globals/job_utils/geomancer')

describe('Geomancer Lasting Emanation host', function()
    it('lowers luopan REGEN_DOWN and routes the action to it', function()
        local regenDown
        local routed
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            getMainLvl = function() return 99 end,
            getMod = function(_, mod)
                assert(mod == xi.mod.REGEN_DOWN)
                return 30
            end,
            setMod = function(_, mod, value)
                assert(mod == xi.mod.REGEN_DOWN)
                regenDown = value
            end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
        }
        local action = {
            ID = function(_, sourceID, targetID) routed = { sourceID, targetID } end,
        }

        assert(xi.job_utils.geomancer.lastingEmanation(player, {}, {}, action) == nil)
        assert(regenDown == 23 and routed[1] == 7 and routed[2] == 42)
    end)
end)
