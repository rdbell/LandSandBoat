require('scripts/globals/job_utils/geomancer')

describe('Geomancer Life Cycle availability host', function()
    it('requires a luopan before enforcing the HP threshold', function()
        local pet = { getPetID = function() return xi.petId.LUOPAN end }
        local hp = 2
        local player = {
            getPet = function() return nil end,
            getHP = function() return hp end,
        }

        assert(xi.job_utils.geomancer.geoOnLifeCycleAbilityCheck(player, {}, {}) == xi.msg.basic.REQUIRE_LUOPAN)

        player.getPet = function() return pet end
        assert(xi.job_utils.geomancer.geoOnLifeCycleAbilityCheck(player, {}, {}) == xi.msg.basic.UNABLE_TO_USE_JA)

        hp = 3
        assert(xi.job_utils.geomancer.geoOnLifeCycleAbilityCheck(player, {}, {}) == 0)
    end)
end)
