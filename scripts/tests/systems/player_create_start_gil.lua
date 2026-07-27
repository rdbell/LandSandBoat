require('scripts/globals/player')

describe('Player creation starting gil', function()
    it('sets gil only when the player is below START_GIL', function()
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
        main.START_GIL = 10
        main.NEW_CHARACTER_CUTSCENE = 1

        local currentGil = 0
        local writes = {}
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function() end,
            getGil = function() return currentGil end,
            setGil = function(_, amount) table.insert(writes, amount) end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        local cases = {
            { gil = 0, expected = { 10 } },
            { gil = 10, expected = {} },
            { gil = 25, expected = {} },
        }
        for _, case in ipairs(cases) do
            currentGil = case.gil
            writes = {}
            xi.player.charCreate(player)
            assert(#writes == #case.expected)
            for i, expected in ipairs(case.expected) do
                assert(writes[i] == expected)
            end
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
