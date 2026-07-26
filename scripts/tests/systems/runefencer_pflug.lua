require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Pflug', function()
    it('uses main-job strength, merit sub-power, and the highest-rune ward color', function()
        local applied
        local info
        local target = { getID = function() return 9 end }
        local player = {
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.MERIT_PFLUG_EFFECT)
                return 4
            end,
            getMainJob = function() return xi.job.RUN end,
            addStatusEffect = function(_, effect, params) applied = { effect, params } end,
        }
        local action = { info = function(_, id, value) info = { id, value } end }

        assert(xi.job_utils.rune_fencer.usePflug(player, target, {}, action) == xi.effect.PFLUG)
        assert(applied[1] == xi.effect.PFLUG)
        assert(applied[2].power == 15 and applied[2].subPower == 4 and applied[2].duration == 120 and applied[2].origin == player)
        assert(info[1] == 9 and info[2] == 1)
    end)
end)
