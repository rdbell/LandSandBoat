require('scripts/globals/job_utils/paladin')

describe('Paladin Divine Emblem', function()
    it('adds the enhanced fixed-duration effect to the player and returns its ID', function()
        local effect
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.ENHANCES_DIVINE_EMBLEM)
                return 20
            end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useDivineEmblem(player, {}, {})

        assert(result == xi.effect.DIVINE_EMBLEM and effect.id == xi.effect.DIVINE_EMBLEM)
        assert(effect.values.power == 70 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
