require('scripts/globals/job_utils/ranger')

describe('Ranger Sharpshot', function()
    it('adds the modifier-adjusted Sharpshot effect and returns its ID', function()
        local effect
        local player = {
            getMod = function() return 12 end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useSharpshot(player, {}, {}, {})

        assert(result == xi.effect.SHARPSHOT and effect.id == xi.effect.SHARPSHOT)
        assert(effect.values.power == 52 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
