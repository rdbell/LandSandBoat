require('scripts/globals/player')

describe('Player creation home-nation ring', function()
    it('grants only a missing ring for the race home nation', function()
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

        local nation = xi.nation.BASTOK
        local hasRing = false
        local rings = {}
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return nation end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function(_, item)
                return item ~= xi.item.BASTOKAN_RING or hasRing
            end,
            addItem = function(_, item)
                assert(item == xi.item.BASTOKAN_RING)
                table.insert(rings, item)
            end,
            addKeyItem = function() end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        xi.player.charCreate(player)
        assert(#rings == 1)

        hasRing = true
        rings = {}
        xi.player.charCreate(player)
        assert(#rings == 0)

        hasRing = false
        nation = xi.nation.SANDORIA
        xi.player.charCreate(player)
        assert(#rings == 0)

        main.ADVANCED_JOB_LEVEL = originalSettings.advancedJob
        main.SUBJOB_QUEST_LEVEL = originalSettings.subjobQuest
        main.ALL_MAPS = originalSettings.allMaps
        main.INITIAL_LEVEL_CAP = originalSettings.levelCap
        main.START_INVENTORY = originalSettings.startInventory
        main.START_GIL = originalSettings.startGil
        main.NEW_CHARACTER_CUTSCENE = originalSettings.cutscene
    end)
end)
