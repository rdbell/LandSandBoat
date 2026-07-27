require('scripts/globals/job_utils/paladin')

describe('Paladin Holy Circle', function()
    it('adds the enhanced main-job effect to the target and returns its ID', function()
        local effect
        local player = {
            getMainJob = function() return xi.job.PLD end,
            getMod = function(_, mod)
                if mod == xi.mod.HOLY_CIRCLE_DURATION then
                    return 20
                end
                assert(mod == xi.mod.HOLY_CIRCLE_POTENCY)
                return 5
            end,
        }
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useHolyCircle(player, target, {})

        assert(result == xi.effect.HOLY_CIRCLE and effect.id == xi.effect.HOLY_CIRCLE)
        assert(effect.values.power == 20 and effect.values.duration == 200 and effect.values.origin == player)
    end)
end)
