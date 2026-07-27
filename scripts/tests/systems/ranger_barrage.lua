require('scripts/globals/job_utils/ranger')

describe('Ranger Barrage', function()
    it('adds the fixed 60-second Barrage effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.ranger.useBarrage(player, {}, {}, {})

        assert(result == xi.effect.BARRAGE and effect.id == xi.effect.BARRAGE)
        assert(effect.values.duration == 60 and effect.values.origin == player)
        assert(effect.values.power == nil and effect.values.tick == nil)
    end)
end)
