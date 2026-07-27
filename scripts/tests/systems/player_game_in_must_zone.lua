require('scripts/globals/player')

describe('Player game-in must-zone cleanup', function()
    it('clears only must-zone variables for other zones', function()
        local writes = {}
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        xi.gear_sets.checkForGearSet = function() end

        local player = {
            getZoneID = function() return 100 end,
            getCharVarsWithSuffix = function(_, suffix)
                assert(suffix == ']mustZone')
                return {
                    ['Quest[1]mustZone'] = 100,
                    ['Quest[2]mustZone'] = 101,
                    ['HQuest[x]mustZone'] = 0,
                }
            end,
            setCharVar = function(_, name, value) writes[name] = value end,
            getQuestStatus = function() return -1 end,
            hasCompletedQuest = function() return false end,
            getCharVar = function() return 0 end,
            setLocalVar = function() end,
            timer = function() end,
        }

        xi.player.onGameIn(player, false, false)
        xi.gear_sets.checkForGearSet = originalCheckForGearSet

        assert(writes['Quest[1]mustZone'] == nil)
        assert(writes['Quest[2]mustZone'] == 0)
        assert(writes['HQuest[x]mustZone'] == 0)
    end)
end)
