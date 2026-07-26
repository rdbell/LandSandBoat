require('scripts/globals/job_utils/dragoon')

describe('Dragoon requires-pet ability check', function()
    it('requires a wyvern, honors actionable checks, and adjusts Spirit Surge recast', function()
        local recast
        local pet = {
            getPetID = function() return xi.petId.WYVERN end,
            canUseAbilities = function() return false end,
        }
        local player = {
            getPet = function() return nil end,
            getMod = function() return 1 end,
        }
        local ability = {
            getID = function() return xi.jobAbility.SPIRIT_SURGE end,
            getRecast = function() return 3600 end,
            setRecast = function(_, value) recast = value end,
        }

        assert(xi.job_utils.dragoon.abilityCheckRequiresPet(player, {}, ability, false) == xi.msg.basic.REQUIRES_A_PET)

        player.getPet = function() return pet end
        assert(xi.job_utils.dragoon.abilityCheckRequiresPet(player, {}, ability, true) == xi.msg.basic.PET_CANNOT_DO_ACTION)

        pet.canUseAbilities = function() return true end
        assert(xi.job_utils.dragoon.abilityCheckRequiresPet(player, {}, ability, true) == 0)
        assert(recast == 3540)
    end)
end)
