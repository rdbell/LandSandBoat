require('scripts/globals/player')

describe('Player creation job unlock settings', function()
    it('unlocks advanced jobs and the subjob independently at level zero', function()
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
        main.ALL_MAPS = 0
        main.INITIAL_LEVEL_CAP = 50
        main.START_INVENTORY = 30
        main.START_GIL = 0
        main.NEW_CHARACTER_CUTSCENE = 1

        local unlocked = {}
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function() end,
            unlockJob = function(_, job) table.insert(unlocked, job) end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        local advanced = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 }
        local cases = {
            { advanced = 1, subjob = 1, expected = {} },
            { advanced = 0, subjob = 1, expected = advanced },
            { advanced = 1, subjob = 0, expected = { 0 } },
            { advanced = 0, subjob = 0, expected = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0 } },
        }
        for _, case in ipairs(cases) do
            main.ADVANCED_JOB_LEVEL = case.advanced
            main.SUBJOB_QUEST_LEVEL = case.subjob
            unlocked = {}
            xi.player.charCreate(player)
            assert(#unlocked == #case.expected)
            for i, expected in ipairs(case.expected) do
                assert(unlocked[i] == expected)
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
