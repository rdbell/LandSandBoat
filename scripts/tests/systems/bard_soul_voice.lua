require('scripts/globals/job_utils/bard')

describe('Bard Soul Voice', function()
    it('adds the fixed Soul Voice effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.bard.useSoulVoice(player, {}, {})

        assert(result == xi.effect.SOUL_VOICE and effect.id == xi.effect.SOUL_VOICE)
        assert(effect.values.power == 1 and effect.values.duration == 180 and effect.values.origin == player)
    end)
end)
