require('scripts/globals/mannequins')

describe('Mannequin pose', function()
    it('selects the race mannequin item and forwards the requested pose', function()
        local itemID, raceArg, poseArg
        local player = {
            setMannequinPose = function(_, item, race, pose)
                itemID, raceArg, poseArg = item, race, pose
            end,
        }
        xi.mannequin.setMannequinPose(player, xi.race.MITHRA, 17)
        assert(itemID == xi.item.MITHRA_MANNEQUIN)
        assert(raceArg == xi.race.MITHRA and poseArg == 17)
    end)
end)
