require('scripts/globals/job_utils/ranger')

describe('Ranger Double Shot', function()
    it('adds the fixed Double Shot effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useDoubleShot(player, {}, {}, {})

        assert(result == xi.effect.DOUBLE_SHOT and effect.id == xi.effect.DOUBLE_SHOT)
        assert(effect.values.power == 40 and effect.values.duration == 90 and effect.values.origin == player)
    end)
end)
