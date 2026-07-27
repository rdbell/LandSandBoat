require('scripts/globals/job_utils/paladin')

describe('Paladin Majesty', function()
    it('adds the fixed Majesty effect to the player and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useMajesty(player, {}, {})

        assert(result == xi.effect.MAJESTY and effect.id == xi.effect.MAJESTY)
        assert(effect.values.power == 25 and effect.values.duration == 180 and effect.values.origin == player)
    end)
end)
