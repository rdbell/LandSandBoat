require('scripts/globals/job_utils/ninja')

describe('Ninja Mikage', function()
    it('adds the fixed Mikage effect to the target and returns its ID', function()
        local effect
        local player = {}
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ninja.useMikage(player, target, {}, {})

        assert(result == xi.effect.MIKAGE and effect.id == xi.effect.MIKAGE)
        assert(effect.values.duration == 45 and effect.values.origin == player)
    end)
end)
