require('scripts/globals/job_utils/monk')

describe('Monk Perfect Counter', function()
    it('adds the fixed self effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.monk.usePerfectCounter(player, {}, {})

        assert(result == xi.effect.PERFECT_COUNTER and effect.id == xi.effect.PERFECT_COUNTER)
        assert(effect.values.power == 2 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
