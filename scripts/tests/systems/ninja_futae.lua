require('scripts/globals/job_utils/ninja')

describe('Ninja Futae', function()
    it('adds the fixed Futae effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ninja.useFutae(player, target, {}, {})

        assert(result == xi.effect.FUTAE and effect.id == xi.effect.FUTAE)
        assert(effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
