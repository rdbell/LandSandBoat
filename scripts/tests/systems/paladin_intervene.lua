require('scripts/globals/job_utils/paladin')

describe('Paladin Intervene', function()
    it('applies its fixed effect and shield/job-point scaled damage', function()
        local effect
        local player = {
            getShieldSize = function() return 3 end,
            getJobPointLevel = function(_, jobPoint)
                assert(jobPoint == xi.jp.INTERVENE_EFFECT)
                return 25
            end,
            getMainLvl = function() return 75 end,
        }
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local damage = xi.job_utils.paladin.useIntervene(player, target, {})

        assert(damage == 365)
        assert(effect.id == xi.effect.INTERVENE)
        assert(effect.values.power == 1 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
