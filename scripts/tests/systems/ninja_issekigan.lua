require('scripts/globals/job_utils/ninja')

describe('Ninja Issekigan', function()
    it('adds the fixed Issekigan effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ninja.useIssekigan(player, target, {}, {})

        assert(result == xi.effect.ISSEKIGAN and effect.id == xi.effect.ISSEKIGAN)
        assert(effect.values.power == 25 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
