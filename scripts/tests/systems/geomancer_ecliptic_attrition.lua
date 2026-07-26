require('scripts/globals/job_utils/geomancer')

describe('Geomancer Ecliptic Attrition host', function()
    it('routes to the luopan, increases REGEN_DOWN, and floors Colure potency once', function()
        local regenDown
        local localVar
        local subPower
        local routed
        local effect = {
            getSubPower = function() return 41 end,
            setSubPower = function(_, value) subPower = value end,
        }
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            getLocalVar = function(_, key)
                assert(key == 'eclipticAttrition')
                return 0
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.REGEN_DOWN)
                return 30
            end,
            getMainLvl = function() return 99 end,
            setMod = function(_, mod, value)
                assert(mod == xi.mod.REGEN_DOWN)
                regenDown = value
            end,
            getStatusEffect = function(_, effectID)
                assert(effectID == xi.effect.COLURE_ACTIVE)
                return effect
            end,
            setLocalVar = function(_, key, value)
                assert(key == 'eclipticAttrition')
                localVar = value
            end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
            hasStatusEffect = function(_, effectID)
                assert(effectID == xi.effect.BOLSTER)
                return false
            end,
        }
        local action = {
            ID = function(_, sourceID, targetID) routed = { sourceID, targetID } end,
        }

        assert(xi.job_utils.geomancer.eclipticAttrition(player, {}, {}, action) == nil)
        assert(routed[1] == 7 and routed[2] == 42)
        assert(regenDown == 36 and subPower == 51 and localVar == 1)
    end)

    it('only routes when Ecliptic Attrition is already active', function()
        local routed
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            getLocalVar = function() return 1 end,
            setMod = function() error('must not mutate REGEN_DOWN') end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
        }
        local action = {
            ID = function(_, sourceID, targetID) routed = { sourceID, targetID } end,
        }

        assert(xi.job_utils.geomancer.eclipticAttrition(player, {}, {}, action) == nil)
        assert(routed[1] == 7 and routed[2] == 42)
    end)

    it('still increases REGEN_DOWN but skips Colure potency under Bolster', function()
        local regenDown
        local localVar
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            getLocalVar = function() return 0 end,
            getMod = function() return 30 end,
            getMainLvl = function() return 99 end,
            setMod = function(_, _, value) regenDown = value end,
            getStatusEffect = function() error('must not read Colure under Bolster') end,
            setLocalVar = function(_, _, value) localVar = value end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
            hasStatusEffect = function(_, effectID)
                assert(effectID == xi.effect.BOLSTER)
                return true
            end,
        }
        local action = { ID = function() end }

        assert(xi.job_utils.geomancer.eclipticAttrition(player, {}, {}, action) == nil)
        assert(regenDown == 36 and localVar == nil)
    end)

    it('still increases REGEN_DOWN but does nothing further without Colure', function()
        local regenDown
        local localVar
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            getLocalVar = function() return 0 end,
            getMod = function() return 30 end,
            getMainLvl = function() return 99 end,
            setMod = function(_, _, value) regenDown = value end,
            getStatusEffect = function(_, effectID)
                assert(effectID == xi.effect.COLURE_ACTIVE)
                return nil
            end,
            setLocalVar = function(_, _, value) localVar = value end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
            hasStatusEffect = function() return false end,
        }
        local action = { ID = function() end }

        assert(xi.job_utils.geomancer.eclipticAttrition(player, {}, {}, action) == nil)
        assert(regenDown == 36 and localVar == nil)
    end)
end)
