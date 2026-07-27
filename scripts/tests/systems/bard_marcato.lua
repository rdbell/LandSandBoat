require('scripts/globals/job_utils/bard')

describe('Bard Marcato', function()
    it('adds the fixed Marcato effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.bard.useMarcato(player, {}, {})

        assert(result == xi.effect.MARCATO and effect.id == xi.effect.MARCATO)
        assert(effect.values.power == 50 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
