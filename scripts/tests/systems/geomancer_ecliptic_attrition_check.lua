require('scripts/globals/job_utils/geomancer')

describe('Geomancer Ecliptic Attrition availability host', function()
    it('requires a luopan and rejects an already active effect', function()
        local noPet = { getPet = function() return nil end }
        assert(xi.job_utils.geomancer.geoOnEclipticAttritionCheck(noPet, {}, {}) == xi.msg.basic.REQUIRE_LUOPAN)

        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getLocalVar = function() return 1 end,
        }
        local active = { getPet = function() return luopan end }
        assert(xi.job_utils.geomancer.geoOnEclipticAttritionCheck(active, {}, {}) == xi.msg.basic.UNABLE_TO_USE_JA)

        luopan.getLocalVar = function() return 0 end
        assert(xi.job_utils.geomancer.geoOnEclipticAttritionCheck(active, {}, {}) == 0)
    end)
end)
