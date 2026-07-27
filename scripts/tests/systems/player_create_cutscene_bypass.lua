require('scripts/globals/player')

describe('Player creation opening-cutscene bypass', function()
    it('grants the coupon and home point only when cutscenes are disabled', function()
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

        local coupons = 0
        local homePoints = 0
        local player = {
            getRace = function() return xi.race.HUME_M end,
            getNation = function() return xi.nation.BASTOK end,
            getMainJob = function() return xi.job.WAR end,
            hasItem = function() return true end,
            addKeyItem = function() end,
            getGil = function() return 0 end,
            addItem = function(_, item)
                assert(item == xi.item.ADVENTURER_COUPON)
                coupons = coupons + 1
            end,
            setHomePoint = function() homePoints = homePoints + 1 end,
            addTitle = function() end,
            setCharVar = function() end,
            setNewPlayer = function() end,
        }

        main.NEW_CHARACTER_CUTSCENE = 1
        xi.player.charCreate(player)
        assert(coupons == 0 and homePoints == 0)

        main.NEW_CHARACTER_CUTSCENE = 0
        xi.player.charCreate(player)
        assert(coupons == 1 and homePoints == 1)

        main.ADVANCED_JOB_LEVEL = originalSettings.advancedJob
        main.SUBJOB_QUEST_LEVEL = originalSettings.subjobQuest
        main.ALL_MAPS = originalSettings.allMaps
        main.INITIAL_LEVEL_CAP = originalSettings.levelCap
        main.START_INVENTORY = originalSettings.startInventory
        main.START_GIL = originalSettings.startGil
        main.NEW_CHARACTER_CUTSCENE = originalSettings.cutscene
    end)
end)
