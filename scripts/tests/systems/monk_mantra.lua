require('scripts/globals/job_utils/monk')

describe('Monk Mantra', function()
    it('replaces MAX_HP_BOOST on the target and returns its ID', function()
        local removed
        local effect
        local player = {
            getMerit = function(_, merit) assert(merit == xi.merit.MANTRA); return 15 end,
        }
        local target = {
            delStatusEffect = function(_, id) removed = id end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.monk.useMantra(player, target, {})

        assert(removed == xi.effect.MAX_HP_BOOST and result == xi.effect.MAX_HP_BOOST and effect.id == xi.effect.MAX_HP_BOOST)
        assert(effect.values.power == 15 and effect.values.duration == 180 and effect.values.origin == player)
    end)
end)
