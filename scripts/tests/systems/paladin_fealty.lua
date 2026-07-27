require('scripts/globals/job_utils/paladin')

describe('Paladin Fealty', function()
    it('adds the merit-enhanced effect to the player and returns its ID', function()
        local effect
        local player = {
            getMerit = function(_, merit)
                assert(merit == xi.merit.FEALTY)
                return 10
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.ENHANCES_FEALTY)
                return 5
            end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useFealty(player, {}, {}, {})

        assert(result == xi.effect.FEALTY and effect.id == xi.effect.FEALTY)
        assert(effect.values.power == 1 and effect.values.duration == 75 and effect.values.origin == player)
    end)
end)
