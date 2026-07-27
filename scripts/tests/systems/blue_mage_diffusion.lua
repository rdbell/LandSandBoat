require('scripts/globals/job_utils/blue_mage')

describe('Blue Mage Diffusion', function()
    it('adds the fixed Diffusion effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.blue_mage.useDiffusion(player, {}, {}, {})

        assert(result == xi.effect.DIFFUSION and effect.id == xi.effect.DIFFUSION)
        assert(effect.values.power == 1 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
