require('scripts/globals/player')

describe('Player creation race gear', function()
    it('adds and equips only missing race-specific gear', function()
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

        local missing = {
            [xi.item.HUME_TUNIC] = true,
            [xi.item.HUME_M_BOOTS] = true,
        }
        local added = {}
        local equipped = {}
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function(_, item) return not missing[item] end,
            addItem = function(_, item) added[item] = (added[item] or 0) + 1 end,
            equipItem = function(_, item) equipped[item] = (equipped[item] or 0) + 1 end,
            addKeyItem = function() end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        xi.player.charCreate(player)
        assert(added[xi.item.HUME_TUNIC] == 1)
        assert(added[xi.item.HUME_M_BOOTS] == 1)
        assert(equipped[xi.item.HUME_TUNIC] == 1)
        assert(equipped[xi.item.HUME_M_BOOTS] == 1)
        assert(added[xi.item.HUME_M_GLOVES] == nil)
        assert(equipped[xi.item.HUME_SLACKS] == nil)

        missing = {}
        added = {}
        equipped = {}
        xi.player.charCreate(player)
        assert(next(added) == nil)
        assert(next(equipped) == nil)

        main.ADVANCED_JOB_LEVEL = originalSettings.advancedJob
        main.SUBJOB_QUEST_LEVEL = originalSettings.subjobQuest
        main.ALL_MAPS = originalSettings.allMaps
        main.INITIAL_LEVEL_CAP = originalSettings.levelCap
        main.START_INVENTORY = originalSettings.startInventory
        main.START_GIL = originalSettings.startGil
        main.NEW_CHARACTER_CUTSCENE = originalSettings.cutscene
    end)
end)
