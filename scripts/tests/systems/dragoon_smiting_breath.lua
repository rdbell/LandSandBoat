require('scripts/globals/job_utils/dragoon')

describe('Dragoon Smiting Breath', function()
    it('sets the reduced recast and dispatches the lowest-resistance breath', function()
        local recast
        local selectedAbility
        local selectedTarget
        local ranks =
        {
            [xi.mod.FIRE_RES_RANK] = 5,
            [xi.mod.ICE_RES_RANK] = 5,
            [xi.mod.WIND_RES_RANK] = 5,
            [xi.mod.EARTH_RES_RANK] = 0,
            [xi.mod.THUNDER_RES_RANK] = 5,
            [xi.mod.WATER_RES_RANK] = 5,
        }
        local target = {
            getMod = function(_, mod) return ranks[mod] end,
        }
        local pet = {
            usePetAbility = function(_, ability, abilityTarget)
                selectedAbility = ability
                selectedTarget = abilityTarget
            end,
        }
        local player = {
            getPet = function() return pet end,
            getMod = function(_, mod)
                if mod == xi.mod.DRAGOON_BREATH_RECAST then return 15 end
                return 0
            end,
        }
        local ability = { getRecast = function() return 60 end }
        local action = { setRecast = function(_, value) recast = value end }

        xi.job_utils.dragoon.useSmitingBreath(player, target, ability, action)

        assert(recast == 45)
        assert(selectedAbility == xi.jobAbility.SAND_BREATH)
        assert(selectedTarget == target)
    end)
end)
