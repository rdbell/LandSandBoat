require('scripts/globals/player')

describe('Player creation starter metadata', function()
    it('sets the New Adventurer title, introduction vars, and new-player flag', function()
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

        local titles = {}
        local vars = {}
        local newPlayer
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function() end,
            getGil = function() return 0 end,
            addTitle = function(_, title) table.insert(titles, title) end,
            setCharVar = function(_, name, value) table.insert(vars, { name, value }) end,
            setNewPlayer = function(_, value) newPlayer = value end,
        }

        xi.player.charCreate(player)

        assert(#titles == 1 and titles[1] == xi.title.NEW_ADVENTURER)
        local expectedVars = {
            { 'HQuest[moghouseExpo]notSeen', 1 },
            { 'spokeKindlix', 1 },
            { 'spokePyropox', 1 },
            { 'EinherjarIntro', 1 },
        }
        assert(#vars == #expectedVars)
        for i, expected in ipairs(expectedVars) do
            assert(vars[i][1] == expected[1] and vars[i][2] == expected[2])
        end
        assert(newPlayer == true)

        main.ADVANCED_JOB_LEVEL = originalSettings.advancedJob
        main.SUBJOB_QUEST_LEVEL = originalSettings.subjobQuest
        main.ALL_MAPS = originalSettings.allMaps
        main.INITIAL_LEVEL_CAP = originalSettings.levelCap
        main.START_INVENTORY = originalSettings.startInventory
        main.START_GIL = originalSettings.startGil
        main.NEW_CHARACTER_CUTSCENE = originalSettings.cutscene
    end)
end)
