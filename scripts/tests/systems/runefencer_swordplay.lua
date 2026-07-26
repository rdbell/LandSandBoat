require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Swordplay', function()
    it('applies its power, Subtle Blow, duration, and tick parameters', function()
        local applied
        local player = {
            getMod = function(_, mod)
                if mod == xi.mod.SWORDPLAY then return 2 end
                if mod == xi.mod.AUGMENTS_SLEIGHT_OF_SWORD then return 5 end
                return 0
            end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.MERIT_SLEIGHT_OF_SWORD)
                return 10
            end,
            addStatusEffect = function(_, effect, params) applied = { effect, params } end,
        }

        assert(xi.job_utils.rune_fencer.useSwordplay(player, {}, {}) == xi.effect.SWORDPLAY)
        assert(applied[1] == xi.effect.SWORDPLAY)
        assert(applied[2].power == 9 and applied[2].subPower == 20)
        assert(applied[2].duration == 120 and applied[2].tick == 3 and applied[2].origin == player)
    end)
end)
