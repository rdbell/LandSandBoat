require('scripts/globals/job_utils/samurai')

describe('Samurai Third Eye host', function()
    it('applies Third Eye to the actor when no image effect is active', function()
        local applied
        local player = {
            hasStatusEffect = function()
                return false
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }
        local ability = {
            setMsg = function()
                error('clear Third Eye must not set a no-effect message')
            end,
        }

        assert(xi.job_utils.samurai.useThirdEye(player, {}, ability) == xi.effect.THIRD_EYE)
        assert(applied.effect == xi.effect.THIRD_EYE)
        assert(applied.params.power == nil and applied.params.duration == 30 and applied.params.origin == player)
    end)

    it('sets no effect without applying when Copy Image or Blink is active', function()
        local additions = 0
        local msg
        local player = {
            hasStatusEffect = function(_, effect)
                return effect == xi.effect.COPY_IMAGE
            end,
            addStatusEffect = function()
                additions = additions + 1
            end,
        }
        local ability = {
            setMsg = function(_, value)
                msg = value
            end,
        }

        assert(xi.job_utils.samurai.useThirdEye(player, {}, ability) == xi.effect.THIRD_EYE)
        assert(additions == 0 and msg == xi.msg.basic.JA_NO_EFFECT)
    end)
end)
