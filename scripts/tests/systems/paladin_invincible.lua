require('scripts/globals/job_utils/paladin')

describe('Paladin Invincible', function()
    it('adds the fixed Invincible effect to the player and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useInvincible(player, {}, {})

        assert(result == xi.effect.INVINCIBLE and effect.id == xi.effect.INVINCIBLE)
        assert(effect.values.power == 1 and effect.values.duration == 30 and effect.values.origin == player)
    end)
end)
