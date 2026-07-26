require('scripts/globals/job_utils/dragoon')

describe('Dragoon Super Jump Spirit Surge effect', function()
    it('lowers the closest behind party member enmity when the dragoon is closer to the mob', function()
        local loweredMember
        local loweredAmount
        local member = {
            getID = function() return 2 end,
            isDead = function() return false end,
            checkDistance = function() return 10 end,
            isBehind = function() return true end,
        }
        local player
        player = {
            getID = function() return 1 end,
            hasStatusEffect = function() return true end,
            getPartyWithTrusts = function() return { player, member } end,
            checkDistance = function() return 5 end,
        }
        local target = {
            isMob = function() return true end,
            lowerEnmity = function(_, entity, amount)
                loweredMember = entity
                loweredAmount = amount
            end,
        }

        xi.job_utils.dragoon.superJumpSurgeEffect(player, target)
        assert(loweredMember == member and loweredAmount == 100)
    end)
end)
