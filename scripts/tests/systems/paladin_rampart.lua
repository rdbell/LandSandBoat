require('scripts/globals/job_utils/paladin')

describe('Paladin Rampart', function()
    it('adds the fixed-power enhanced-duration effect to the target and returns its ID', function()
        local effect
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.RAMPART_DURATION)
                return 10
            end,
        }
        local target = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useRampart(player, target, {})

        assert(result == xi.effect.RAMPART and effect.id == xi.effect.RAMPART)
        assert(effect.values.power == 2500 and effect.values.duration == 40 and effect.values.origin == player)
    end)
end)
