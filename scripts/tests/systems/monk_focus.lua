require('scripts/globals/job_utils/monk')

describe('Monk Focus', function()
    it('adds the target-derived self effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }
        local target = {
            getMod = function(_, mod) assert(mod == xi.mod.FOCUS_EFFECT); return 6 end,
            getJobPointLevel = function(_, jp) assert(jp == xi.jp.FOCUS_EFFECT); return 4 end,
        }

        local result = xi.job_utils.monk.useFocus(player, target, {})

        assert(result == xi.effect.FOCUS and effect.id == xi.effect.FOCUS)
        assert(effect.values.power == 10 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
