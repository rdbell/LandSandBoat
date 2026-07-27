require('scripts/globals/job_utils/monk')

describe('Monk Formless Strikes', function()
    it('adds the fixed self effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.monk.useFormlessStrikes(player, {}, {})

        assert(result == xi.effect.FORMLESS_STRIKES and effect.id == xi.effect.FORMLESS_STRIKES)
        assert(effect.values.power == 1 and effect.values.duration == 180 and effect.values.origin == player)
    end)
end)
