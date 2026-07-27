require('scripts/globals/job_utils/ranger')

describe('Ranger Velocity Shot', function()
    it('adds the fixed Velocity Shot effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useVelocityShot(player, {}, {}, {})

        assert(result == xi.effect.VELOCITY_SHOT and effect.id == xi.effect.VELOCITY_SHOT)
        assert(effect.values.power == 1 and effect.values.duration == 7200 and effect.values.origin == player)
    end)
end)
