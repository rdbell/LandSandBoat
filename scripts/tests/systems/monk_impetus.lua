require('scripts/globals/job_utils/monk')

describe('Monk Impetus', function()
    it('adds the fixed self effect with augmented sub-power', function()
        local effect
        local player = {
            getMod = function(_, mod) assert(mod == xi.mod.AUGMENTS_IMPETUS); return 7 end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.monk.useImpetus(player, {}, {})

        assert(result == xi.effect.IMPETUS and effect.id == xi.effect.IMPETUS)
        assert(effect.values.power == 0 and effect.values.subPower == 7 and effect.values.duration == 180 and effect.values.origin == player)
    end)
end)
