require('scripts/globals/job_utils/ranger')

describe('Ranger Stealth Shot', function()
    it('adds the fixed Stealth Shot effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useStealthShot(player, {}, {}, {})

        assert(result == xi.effect.STEALTH_SHOT and effect.id == xi.effect.STEALTH_SHOT)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
