require('scripts/globals/job_utils/ranger')

describe('Ranger Flashy Shot', function()
    it('adds the fixed Flashy Shot effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useFlashyShot(player, {}, {}, {})

        assert(result == xi.effect.FLASHY_SHOT and effect.id == xi.effect.FLASHY_SHOT)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
