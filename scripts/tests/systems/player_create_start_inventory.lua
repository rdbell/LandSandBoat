require('scripts/globals/player')

describe('Player creation starting inventory', function()
    it('expands inventory and satchel only above the default size', function()
        local changes = {}
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
        main.START_GIL = 0
        main.NEW_CHARACTER_CUTSCENE = 1

        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function() end,
            changeContainerSize = function(_, container, amount)
                table.insert(changes, { container, amount })
            end,
            getGil = function() return 0 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        local cases = {
            { size = 30, expected = {} },
            { size = 31, expected = { { xi.inv.INVENTORY, 1 }, { xi.inv.MOGSATCHEL, 31 } } },
            { size = 80, expected = { { xi.inv.INVENTORY, 50 }, { xi.inv.MOGSATCHEL, 80 } } },
        }
        for _, case in ipairs(cases) do
            main.START_INVENTORY = case.size
            changes = {}
            xi.player.charCreate(player)
            assert(#changes == #case.expected)
            for i, expected in ipairs(case.expected) do
                assert(changes[i][1] == expected[1] and changes[i][2] == expected[2])
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
