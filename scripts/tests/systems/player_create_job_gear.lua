require('scripts/globals/player')

describe('Player creation starter job gear', function()
    it('adds only missing starter job items', function()
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

        local job = xi.job.WAR
        local missing = {}
        local items = {}
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return job end,
            hasItem = function(_, item) return not missing[item] end,
            addItem = function(_, item) table.insert(items, item) end,
            addKeyItem = function() end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        missing[xi.item.ONION_SWORD] = true
        xi.player.charCreate(player)
        assert(#items == 1)
        assert(items[1] == xi.item.ONION_SWORD)

        missing = {}
        items = {}
        xi.player.charCreate(player)
        assert(#items == 0)

        job = xi.job.WHM
        missing[xi.item.ONION_ROD] = true
        xi.player.charCreate(player)
        assert(#items == 1)
        assert(items[1] == xi.item.ONION_ROD)

        main.ADVANCED_JOB_LEVEL = originalSettings.advancedJob
        main.SUBJOB_QUEST_LEVEL = originalSettings.subjobQuest
        main.ALL_MAPS = originalSettings.allMaps
        main.INITIAL_LEVEL_CAP = originalSettings.levelCap
        main.START_INVENTORY = originalSettings.startInventory
        main.START_GIL = originalSettings.startGil
        main.NEW_CHARACTER_CUTSCENE = originalSettings.cutscene
    end)
end)
