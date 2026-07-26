require('scripts/globals/job_utils/dragoon')

describe('Dragoon Damage Breath selection', function()
    it('dispatches the breath with the target lowest elemental resistance rank', function()
        local selectedAbility
        local selectedTarget
        local ranks =
        {
            [xi.mod.FIRE_RES_RANK] = 5,
            [xi.mod.ICE_RES_RANK] = 1,
            [xi.mod.WIND_RES_RANK] = 5,
            [xi.mod.EARTH_RES_RANK] = 5,
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
        }

        xi.job_utils.dragoon.pickAndUseDamageBreath(player, target)

        assert(selectedAbility == xi.jobAbility.FROST_BREATH)
        assert(selectedTarget == target)
    end)
end)
