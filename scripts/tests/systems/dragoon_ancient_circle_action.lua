require('scripts/globals/job_utils/dragoon')

describe('Dragoon Ancient Circle action', function()
    it('sets its fortified-dragons message and applies the resolved status effect', function()
        local message
        local appliedEffect
        local appliedParams
        local player = {
            getMod = function(_, mod)
                if mod == xi.mod.ANCIENT_CIRCLE_DURATION then return 30 end
                return 2
            end,
            getMainJob = function() return xi.job.DRG end,
            getJobPointLevel = function() return 5 end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                appliedEffect = effect
                appliedParams = params
            end,
        }
        local ability = { setMsg = function(_, msg) message = msg end }

        assert(xi.job_utils.dragoon.useAncientCircle(player, target, ability) == xi.effect.ANCIENT_CIRCLE)
        assert(message == xi.msg.basic.USES_ABILITY_FORTIFIED_DRAGONS)
        assert(appliedEffect == xi.effect.ANCIENT_CIRCLE)
        assert(appliedParams.power == 22 and appliedParams.duration == 210 and appliedParams.origin == player)
    end)
end)
