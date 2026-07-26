require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer rune enchantment', function()
    it('evicts the oldest rune at capacity and applies the selected enhancement', function()
        local removed = false
        local applied
        local target = {
            getMainJob = function() return xi.job.RUN end,
            getMainLvl = function() return 65 end,
            getActiveRuneCount = function() return 3 end,
            removeOldestRune = function() removed = true end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.MERIT_RUNE_ENHANCE)
                return 2
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.RUNE_ENCHANTMENT_EFFECT)
                return 3
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect, params }
            end,
        }

        xi.job_utils.rune_fencer.useRuneEnchantment({}, target, {}, xi.effect.IGNIS)

        assert(removed)
        assert(applied[1] == xi.effect.IGNIS)
        assert(applied[2].power == 42 and applied[2].duration == 300 and applied[2].origin == target)
    end)
end)
