require('scripts/globals/job_utils/geomancer')

describe('Geomancer Bolster effect adjustments', function()
    it('doubles active Indi potency on gain and restores matching boosted values on loss', function()
        local indiSubPower = 4
        local geoSubPower = 10
        local regenDown
        local indiEffect = { setSubPower = function(_, value) indiSubPower = value end, getSubPower = function() return indiSubPower end }
        local geoEffect = { setSubPower = function(_, value) geoSubPower = value end, getSubPower = function() return geoSubPower end }
        local pet = {
            getPetID = function() return xi.petId.LUOPAN end,
            getLocalVar = function() return 5 end,
            getStatusEffect = function() return geoEffect end,
            getMod = function() return -20 end,
            setMod = function(_, _, value) regenDown = value end,
        }
        local target = {
            hasStatusEffect = function() return true end,
            getLocalVar = function() return 4 end,
            getStatusEffect = function() return indiEffect end,
            getJobPointLevel = function() return 3 end,
            getPet = function() return pet end,
        }

        xi.job_utils.geomancer.bolsterOnEffectGain(target, {})
        assert(indiSubPower == 8)

        xi.job_utils.geomancer.bolsterOnEffectLose(target, {})
        assert(geoSubPower == 5)
        assert(indiSubPower == 4)
        assert(regenDown == -17)
    end)
end)
