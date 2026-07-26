require('scripts/globals/job_utils/geomancer')

describe('Geomancer Theurgic Focus availability host', function()
    it('rejects an already active effect and allows a fresh use', function()
        local player = { hasStatusEffect = function() return true end }
        assert(xi.job_utils.geomancer.geoOnTheurgicFocusCheck(player, {}, {}) == xi.msg.basic.EFFECT_ALREADY_ACTIVE)

        player.hasStatusEffect = function() return false end
        assert(xi.job_utils.geomancer.geoOnTheurgicFocusCheck(player, {}, {}) == 0)
    end)
end)
