require('scripts/globals/job_utils/dragoon')

describe('Dragoon Fly High plan', function()
    it('resets all jump recasts and applies Fly High', function()
        local resetRecasts = {}
        local player = {
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.FLY_HIGH)
                assert(params.duration == 30)
            end,
        }
        local target = {
            resetRecast = function(_, recastType, recastID)
                assert(recastType == xi.recast.ABILITY)
                table.insert(resetRecasts, recastID)
            end,
        }

        assert(xi.job_utils.dragoon.useFlyHigh(player, target, {}) == xi.effect.FLY_HIGH)
        assert(#resetRecasts == 5)
        assert(resetRecasts[1] == 158)
        assert(resetRecasts[2] == 159)
        assert(resetRecasts[3] == 160)
        assert(resetRecasts[4] == 166)
        assert(resetRecasts[5] == 167)
    end)
end)
