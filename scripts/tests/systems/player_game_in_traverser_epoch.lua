require('scripts/globals/player')

describe('Player game-in traverser epoch migration', function()
    it('initializes the epoch only for a completed Journey Begins quest without one', function()
        local initialized = 0
        local completed = true
        local epoch = 0
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        xi.gear_sets.checkForGearSet = function() end

        local player = {
            getZoneID = function() return 100 end,
            getCharVarsWithSuffix = function() return {} end,
            getQuestStatus = function() return -1 end,
            hasCompletedQuest = function() return completed end,
            getTraverserEpoch = function() return epoch end,
            setTraverserEpoch = function() initialized = initialized + 1 end,
            getCharVar = function() return 0 end,
            setLocalVar = function() end,
            timer = function() end,
        }

        xi.player.onGameIn(player, false, false)
        assert(initialized == 1)

        epoch = 1
        xi.player.onGameIn(player, false, false)
        assert(initialized == 1)

        epoch = 0
        completed = false
        xi.player.onGameIn(player, false, false)
        xi.gear_sets.checkForGearSet = originalCheckForGearSet
        assert(initialized == 1)
    end)
end)
