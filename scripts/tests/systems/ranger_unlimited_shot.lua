require('scripts/globals/job_utils/ranger')

describe('Ranger Unlimited Shot', function()
    it('adds the fixed Unlimited Shot effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useUnlimitedShot(player, {}, {}, {})

        assert(result == xi.effect.UNLIMITED_SHOT and effect.id == xi.effect.UNLIMITED_SHOT)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
