require('scripts/globals/mannequins')

describe('Mannequin get pose', function()
    it('reads the pose from the race mannequin item', function()
        local itemID
        local player = {
            getMannequinPose = function(_, item)
                itemID = item
                return 9
            end,
        }
        assert(xi.mannequin.getMannequinPose(player, xi.race.GALKA) == 9)
        assert(itemID == xi.item.GALKA_MANNEQUIN)
    end)
end)
