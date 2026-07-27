require('scripts/globals/job_utils/paladin')

describe('Paladin Sepulcher', function()
    it('adds the enhanced-duration effect to the target without returning an effect ID', function()
        local effect
        local player = {
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.SEPULCHER_DURATION)
                return 10
            end,
        }
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useSepulcher(player, target, {})

        assert(result == nil and effect.id == xi.effect.SEPULCHER)
        assert(effect.values.power == 20 and effect.values.duration == 190 and effect.values.origin == player)
    end)
end)
