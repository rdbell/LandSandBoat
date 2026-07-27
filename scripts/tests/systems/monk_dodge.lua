require('scripts/globals/job_utils/monk')

describe('Monk Dodge', function()
    it('adds the target-derived self effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }
        local target = {
            getMod = function(_, mod) assert(mod == xi.mod.DODGE_EFFECT); return 5 end,
            getJobPointLevel = function(_, jp) assert(jp == xi.jp.DODGE_EFFECT); return 7 end,
        }

        local result = xi.job_utils.monk.useDodge(player, target, {})

        assert(result == xi.effect.DODGE and effect.id == xi.effect.DODGE)
        assert(effect.values.power == 12 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
