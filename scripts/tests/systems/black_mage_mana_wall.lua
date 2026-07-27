require('scripts/globals/job_utils/black_mage')

describe('Black Mage Mana Wall', function()
    it('adds the fixed Mana Wall effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.black_mage.useManaWall(player, {}, {})

        assert(result == xi.effect.MANA_WALL and effect.id == xi.effect.MANA_WALL)
        assert(effect.values.power == 1 and effect.values.duration == 300 and effect.values.origin == player)
    end)
end)
