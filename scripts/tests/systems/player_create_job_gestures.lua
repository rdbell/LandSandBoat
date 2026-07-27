require('scripts/globals/player')

describe('Player creation job gestures', function()
    it('grants the six starter job gestures in declaration order', function()
        local main = xi.settings.main
        local originalSettings = {
            advancedJob = main.ADVANCED_JOB_LEVEL,
            subjobQuest = main.SUBJOB_QUEST_LEVEL,
            allMaps = main.ALL_MAPS,
            levelCap = main.INITIAL_LEVEL_CAP,
            startInventory = main.START_INVENTORY,
            startGil = main.START_GIL,
            cutscene = main.NEW_CHARACTER_CUTSCENE,
        }
        main.ADVANCED_JOB_LEVEL = 1
        main.SUBJOB_QUEST_LEVEL = 1
        main.ALL_MAPS = 0
        main.INITIAL_LEVEL_CAP = 50
        main.START_INVENTORY = 30
        main.START_GIL = 0
        main.NEW_CHARACTER_CUTSCENE = 1

        local keyItems = {}
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function(_, keyItem) table.insert(keyItems, keyItem) end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        xi.player.charCreate(player)

        local expected = {
            xi.ki.JOB_GESTURE_WARRIOR,
            xi.ki.JOB_GESTURE_MONK,
            xi.ki.JOB_GESTURE_WHITE_MAGE,
            xi.ki.JOB_GESTURE_BLACK_MAGE,
            xi.ki.JOB_GESTURE_RED_MAGE,
            xi.ki.JOB_GESTURE_THIEF,
        }
        assert(#keyItems == #expected + 1)
        for i, keyItem in ipairs(expected) do
            assert(keyItems[i + 1] == keyItem)
        end

        main.ADVANCED_JOB_LEVEL = originalSettings.advancedJob
        main.SUBJOB_QUEST_LEVEL = originalSettings.subjobQuest
        main.ALL_MAPS = originalSettings.allMaps
        main.INITIAL_LEVEL_CAP = originalSettings.levelCap
        main.START_INVENTORY = originalSettings.startInventory
        main.START_GIL = originalSettings.startGil
        main.NEW_CHARACTER_CUTSCENE = originalSettings.cutscene
    end)
end)
