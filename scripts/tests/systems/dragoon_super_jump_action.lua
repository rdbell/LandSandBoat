require('scripts/globals/job_utils/dragoon')

describe('Dragoon Super Jump action', function()
    it('makes the player airborne and requests Super Climb from an active wyvern', function()
        local airDuration
        local petAbility
        local wyvern = {
            getPetID = function() return xi.petId.WYVERN end,
            getHP = function() return 1 end,
            isEngaged = function() return true end,
            usePetAbility = function(_, ability) petAbility = ability end,
        }
        local player
        player = {
            getNotorietyList = function() return {} end,
            queue = function(_, _, callback) callback(player) end,
            untargetableAndUnactionable = function(_, duration) airDuration = duration end,
            getPet = function() return wyvern end,
            hasStatusEffect = function() return false end,
        }
        local ability = { setMsg = function() end }

        xi.job_utils.dragoon.useSuperJump(player, {}, ability)
        assert(airDuration == 5000)
        assert(petAbility == xi.jobAbility.SUPER_CLIMB)
    end)
end)
