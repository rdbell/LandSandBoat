require('scripts/globals/job_utils/ranger')

describe('Ranger Overkill', function()
    it('adds the fixed Overkill effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useOverkill(player, {}, {}, {})

        assert(result == xi.effect.OVERKILL and effect.id == xi.effect.OVERKILL)
        assert(effect.values.power == 11 and effect.values.duration == 60 and effect.values.tick == 1)
        assert(effect.values.origin == player)
    end)
end)
