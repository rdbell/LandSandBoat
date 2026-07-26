require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Battuta', function()
    it('applies its powers, duration, and highest-rune ward color', function()
        local applied
        local info
        local target = {
            getActiveRuneCount = function() return 2 end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getID = function() return 9 end,
            addStatusEffect = function(_, effect, params) applied = { effect, params } end,
        }
        local player = {
            getMerit = function(_, merit)
                assert(merit == xi.merit.MERIT_BATTUTA)
                return 4
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.ENHANCES_BATTUTA)
                return 4
            end,
        }
        local action = { info = function(_, id, value) info = { id, value } end }

        assert(xi.job_utils.rune_fencer.useBattuta(player, target, {}, action) == xi.effect.BATTUTA)
        assert(applied[1] == xi.effect.BATTUTA)
        assert(applied[2].power == 40 and applied[2].subPower == 20 and applied[2].duration == 90 and applied[2].origin == player)
        assert(info[1] == 9 and info[2] == 1)
    end)
end)
