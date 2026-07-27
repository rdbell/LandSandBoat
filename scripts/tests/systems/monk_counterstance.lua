require('scripts/globals/job_utils/monk')

describe('Monk Counterstance', function()
    it('replaces the target effect with player-origin power and returns its ID', function()
        local removed
        local added
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.COUNTERSTANCE_EFFECT)
                return 7
            end,
        }
        local target = {
            delStatusEffect = function(_, effect)
                removed = effect
            end,
            addStatusEffect = function(_, effect, values)
                added = { effect = effect, values = values }
            end,
        }

        local result = xi.job_utils.monk.useCounterstance(player, target, {})

        assert(result == xi.effect.COUNTERSTANCE and removed == xi.effect.COUNTERSTANCE)
        assert(added.effect == xi.effect.COUNTERSTANCE)
        assert(added.values.power == 52 and added.values.duration == 300 and added.values.origin == player)
    end)
end)
