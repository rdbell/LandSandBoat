require('scripts/globals/job_utils/ranger')

describe('Ranger Scavenge Fire and Brimstone item', function()
    local function tryScavengeQuestItem(params)
        local given = {}
        local player = {
            getCharVar = function() return params.questStage or 5 end,
            getZoneID = function() return params.zone or xi.zone.CASTLE_OZTROJA end,
            hasItem = function() return params.hasEarring or false end,
            getYPos = function() return params.y or -40 end,
            getXPos = function() return params.x or -80 end,
            getZPos = function() return params.z or -80 end,
        }
        local oldRandom = math.random
        local oldGiveItem = npcUtil.giveItem
        math.random = function(low, high)
            assert(low == 1 and high == 100)
            return params.roll or 50
        end
        npcUtil.giveItem = function(_, item)
            table.insert(given, item)
        end

        local found = xi.job_utils.ranger.tryScavengeQuestItem(player)

        math.random = oldRandom
        npcUtil.giveItem = oldGiveItem
        return found, given
    end

    it('grants the Old Earring at the quest location through the inclusive 50 percent roll', function()
        local found, given = tryScavengeQuestItem({ roll = 50 })

        assert(found)
        assert(#given == 1 and given[1] == xi.item.OLD_EARRING)
    end)

    it('requires the quest state, missing earring, strict position bounds, and a successful roll', function()
        for _, params in ipairs({
            { questStage = 4 },
            { zone = xi.zone.CASTLE_OZTROJA + 1 },
            { hasEarring = true },
            { y = -43 },
            { y = -38 },
            { x = -85 },
            { x = -73 },
            { z = -85 },
            { z = -75 },
            { roll = 51 },
        }) do
            local found, given = tryScavengeQuestItem(params)
            assert(not found and #given == 0)
        end
    end)
end)
