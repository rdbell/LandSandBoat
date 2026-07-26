require('scripts/globals/job_utils/geomancer')

describe('Geomancer Bolster host', function()
    it('adds the modified self Bolster request and returns its effect', function()
        local applied
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.BOLSTER_EFFECT)
                return 20
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.geomancer.bolster(player, {}, {}) == xi.effect.BOLSTER)
        assert(applied.effect == xi.effect.BOLSTER)
        assert(applied.params.duration == 260 and applied.params.tick == 3 and applied.params.origin == player)
    end)
end)
