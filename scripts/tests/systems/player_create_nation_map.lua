require('scripts/globals/player')

describe('Player creation nation map', function()
    it('grants the starting nation area map before job gestures', function()
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

        local nation = xi.nation.SANDORIA
        local keyItems = {}
        local player = {
            getRace = function() return xi.race.ELVAAN_M end,
            getNation = function() return nation end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function(_, keyItem) table.insert(keyItems, keyItem) end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        for _, case in ipairs({
            { nation = xi.nation.SANDORIA, map = xi.ki.MAP_OF_THE_SAN_DORIA_AREA },
            { nation = xi.nation.BASTOK, map = xi.ki.MAP_OF_THE_BASTOK_AREA },
            { nation = xi.nation.WINDURST, map = xi.ki.MAP_OF_THE_WINDURST_AREA },
        }) do
            nation = case.nation
            keyItems = {}
            xi.player.charCreate(player)
            assert(keyItems[1] == case.map)
            assert(#keyItems == 7)
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
