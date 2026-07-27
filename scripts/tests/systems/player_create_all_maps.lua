require('scripts/globals/player')

describe('Player creation all-maps setting', function()
    it('grants all map key items only when ALL_MAPS equals one', function()
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

        main.ALL_MAPS = 0
        xi.player.charCreate(player)
        assert(#keyItems == 7) -- nation map plus six job gestures

        main.ALL_MAPS = 1
        keyItems = {}
        xi.player.charCreate(player)
        assert(#keyItems == 140) -- seven unconditional key items plus 133 maps
        assert(keyItems[8] == xi.ki.MAP_OF_THE_SAN_DORIA_AREA)
        assert(keyItems[70] == xi.ki.MAP_OF_DIO_ABDHALJS_GHELSBA)
        assert(keyItems[71] == xi.ki.MAP_OF_AL_ZAHBI)
        assert(keyItems[133] == xi.ki.MAP_OF_RAKAZNAR)
        assert(keyItems[134] == xi.ki.MAP_OF_RALA_WATERWAYS_U)
        assert(keyItems[137] == xi.ki.MAP_OF_RAKAZNAR_U)
        assert(keyItems[138] == xi.ki.MAP_OF_ESCHA_ZITAH)
        assert(keyItems[140] == xi.ki.MAP_OF_REISENJIMA)
        for _, keyItem in ipairs(keyItems) do
            assert(keyItem ~= 2306)
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
