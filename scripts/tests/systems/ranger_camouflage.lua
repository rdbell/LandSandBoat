require('scripts/globals/job_utils/ranger')

describe('Ranger Camouflage', function()
    local function useCamouflage(params)
        local effect
        local oldRandom = math.random
        math.random = function(low, high)
            assert(low == 30 and high == 300)
            return params.roll
        end
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.CAMOUFLAGE_DURATION)
                return params.durationMod or 0
            end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }
        local result = xi.job_utils.ranger.useCamouflage(player, {}, {}, {})
        math.random = oldRandom
        return result, effect
    end

    it('uses the minimum random roll and fixed Camouflage power', function()
        local result, effect = useCamouflage({ roll = 30 })
        assert(result == xi.effect.CAMOUFLAGE and effect.id == xi.effect.CAMOUFLAGE)
        assert(effect.values.power == 1 and effect.values.duration == 30)
    end)

    it('applies the duration modifier to the maximum random roll', function()
        local result, effect = useCamouflage({ roll = 300, durationMod = 25 })
        assert(result == xi.effect.CAMOUFLAGE and effect.values.duration == 375)
    end)
end)
