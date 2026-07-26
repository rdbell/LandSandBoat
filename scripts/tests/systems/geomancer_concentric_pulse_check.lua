require('scripts/globals/job_utils/geomancer')

describe('Geomancer Concentric Pulse availability host', function()
    it('requires a luopan and checks player and target range separately', function()
        local pet = {
            getPetID = function() return xi.petId.LUOPAN end,
            getHitboxSize = function() return 1 end,
            getTargID = function() return 42 end,
        }
        local playerDistance = 8
        local targetDistance = 9
        local player = {
            getPet = function() return pet end,
            checkDistance = function() return playerDistance end,
            getHitboxSize = function() return 2 end,
        }
        local target = {
            checkDistance = function() return targetDistance end,
            getHitboxSize = function() return 3 end,
        }
        local ability = { getRange = function() return 5 end }

        player.getPet = function() return nil end
        assert(xi.job_utils.geomancer.geoOnConcentricPulseAbilityCheck(player, target, ability) == xi.msg.basic.REQUIRE_LUOPAN)

        player.getPet = function() return pet end
        assert(xi.job_utils.geomancer.geoOnConcentricPulseAbilityCheck(player, target, ability) == 0)

        playerDistance = 8.1
        assert(xi.job_utils.geomancer.geoOnConcentricPulseAbilityCheck(player, target, ability) == xi.msg.basic.TARG_OUT_OF_RANGE_2)

        playerDistance = 8
        targetDistance = 9.1
        local msg, param = xi.job_utils.geomancer.geoOnConcentricPulseAbilityCheck(player, target, ability)
        assert(msg == xi.msg.basic.TARG_OUT_OF_RANGE_2)
        assert(param == 42)
    end)
end)
