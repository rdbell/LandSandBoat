require('scripts/globals/job_utils/monk')

describe('Monk Inner Strength', function()
    it('applies one-hour recast reduction and its fixed self effect', function()
        local recast
        local effect
        local player = {
            getMod = function(_, mod) assert(mod == xi.mod.ONE_HOUR_RECAST); return 10 end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }
        local ability = {
            getRecast = function() return 3600 end,
            setRecast = function(_, value) recast = value end,
        }

        local msg, status = xi.job_utils.monk.checkInnerStrength(player, {}, ability)
        local result = xi.job_utils.monk.useInnerStrength(player, {}, ability)

        assert(msg == 0 and status == 0 and recast == 3000)
        assert(result == xi.effect.INNER_STRENGTH and effect.id == xi.effect.INNER_STRENGTH)
        assert(effect.values.power == 2 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
