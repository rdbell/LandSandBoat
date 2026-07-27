require('scripts/globals/player')

describe('Player game-in missing Visitant safeguard', function()
    it('evacuates only a game-login player without Visitant from an Abyssea zone', function()
        local messages = 0
        local positions = 0
        local gameLogin = true
        local hasVisitant = false
        local region = xi.region.ABYSSEA
        local zoneID = xi.zone.ABYSSEA_ATTOHWA
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        xi.gear_sets.checkForGearSet = function() end

        local player = {
            getLocalVar = function() return gameLogin and 1 or 0 end,
            getCurrentRegion = function() return region end,
            hasStatusEffect = function() return hasVisitant end,
            getZoneID = function() return zoneID end,
            messageSpecial = function(_, messageID)
                assert(messageID == 7254)
                messages = messages + 1
            end,
            setPos = function(_, ...)
                local position = { ... }
                local expected = xi.abyssea.exitPositions[zoneID]
                for i, value in ipairs(expected) do
                    assert(position[i] == value)
                end
                positions = positions + 1
            end,
            getCharVarsWithSuffix = function() return {} end,
            getQuestStatus = function() return -1 end,
            hasCompletedQuest = function() return false end,
            getCharVar = function() return 0 end,
            setLocalVar = function() end,
            timer = function() end,
        }

        xi.player.onGameIn(player, false, true)
        assert(messages == 1 and positions == 1)

        gameLogin = false
        xi.player.onGameIn(player, false, true)
        assert(messages == 1 and positions == 1)

        gameLogin = true
        hasVisitant = true
        xi.player.onGameIn(player, false, true)
        assert(messages == 1 and positions == 1)

        hasVisitant = false
        region = 0
        xi.player.onGameIn(player, false, true)
        xi.gear_sets.checkForGearSet = originalCheckForGearSet
        assert(messages == 1 and positions == 1)
    end)
end)
