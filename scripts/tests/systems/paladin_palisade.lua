require('scripts/globals/job_utils/paladin')

describe('Paladin Palisade', function()
    it('adds the job-point-enhanced effect to the player and returns its ID', function()
        local effect
        local player = {
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.PALISADE_EFFECT)
                return 5
            end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.usePalisade(player, {}, {})

        assert(result == xi.effect.PALISADE and effect.id == xi.effect.PALISADE)
        assert(effect.values.power == 35 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
