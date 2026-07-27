require('scripts/globals/job_utils/ranger')

describe('Ranger Decoy Shot', function()
    it('adds the fixed Decoy Shot effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useDecoyShot(player, target, {}, {})

        assert(result == xi.effect.DECOY_SHOT and effect.id == xi.effect.DECOY_SHOT)
        assert(effect.values.power == 11 and effect.values.duration == 30 and effect.values.tick == 1)
        assert(effect.values.origin == player)
    end)
end)
