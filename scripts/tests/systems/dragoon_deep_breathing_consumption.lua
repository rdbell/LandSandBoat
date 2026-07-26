require('scripts/globals/job_utils/dragoon')

describe('Dragoon Deep Breathing consumption', function()
    it('uses the healing bonus and removes Magic Attack Boost when present', function()
        local removed
        local master = {
            getMerit = function() return 3 end,
            getMod = function() return 1 end,
        }
        local wyvern = {
            hasStatusEffect = function(_, effect) return effect == xi.effect.MAGIC_ATK_BOOST end,
            delStatusEffect = function(_, effect) removed = effect end,
        }

        assert(xi.job_utils.dragoon.getDeepBreathingBonus(wyvern, master, true) == 90)
        assert(removed == xi.effect.MAGIC_ATK_BOOST)
    end)
end)
