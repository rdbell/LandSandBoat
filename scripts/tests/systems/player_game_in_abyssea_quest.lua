require('scripts/globals/player')

describe('Player game-in Abyssea quest promotion', function()
    it('adds A Journey Begins only when Abyssea is enabled and the quest is available', function()
        local added = 0
        local questStatus = xi.questStatus.QUEST_AVAILABLE
        local originalEnableAbyssea = xi.settings.main.ENABLE_ABYSSEA
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        xi.settings.main.ENABLE_ABYSSEA = 1
        xi.gear_sets.checkForGearSet = function() end

        local player = {
            getZoneID = function() return 100 end,
            getCharVarsWithSuffix = function() return {} end,
            getQuestStatus = function() return questStatus end,
            addQuest = function(_, logID, questID)
                assert(logID == xi.questLog.ABYSSEA)
                assert(questID == xi.quest.id.abyssea.A_JOURNEY_BEGINS)
                added = added + 1
            end,
            hasCompletedQuest = function() return false end,
            getCharVar = function() return 0 end,
            setLocalVar = function() end,
            timer = function() end,
        }

        xi.player.onGameIn(player, false, false)
        assert(added == 1)

        questStatus = xi.questStatus.QUEST_ACCEPTED
        xi.player.onGameIn(player, false, false)
        assert(added == 1)

        questStatus = xi.questStatus.QUEST_AVAILABLE
        xi.settings.main.ENABLE_ABYSSEA = 0
        xi.player.onGameIn(player, false, false)
        xi.settings.main.ENABLE_ABYSSEA = originalEnableAbyssea
        xi.gear_sets.checkForGearSet = originalCheckForGearSet
        assert(added == 1)
    end)
end)
