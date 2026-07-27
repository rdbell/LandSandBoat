require('scripts/globals/job_utils/monk')

describe('Monk Boost', function()
    it('adds a player-origin effect when none is active', function()
        local added
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.BOOST_EFFECT)
                return 10
            end,
            hasStatusEffect = function()
                return false
            end,
            addStatusEffect = function(_, effect, values)
                added = { effect = effect, values = values }
            end,
        }

        xi.job_utils.monk.useBoost(player, {}, {})

        assert(added.effect == xi.effect.BOOST)
        assert(added.values.power == 13.5 and added.values.duration == 180 and added.values.origin == player)
    end)

    it('stacks stored power and ATTP when the effect is active', function()
        local setPower
        local addedMod
        local effect = {
            getPower = function()
                return 12.5
            end,
            setPower = function(_, power)
                setPower = power
            end,
            addMod = function(_, mod, power)
                addedMod = { mod = mod, power = power }
            end,
        }
        local player = {
            getMod = function()
                return 10
            end,
            hasStatusEffect = function()
                return true
            end,
            getStatusEffect = function(_, effectID)
                assert(effectID == xi.effect.BOOST)
                return effect
            end,
        }

        xi.job_utils.monk.useBoost(player, {}, {})

        assert(setPower == 26 and addedMod.mod == xi.mod.ATTP and addedMod.power == 13.5)
    end)
end)
