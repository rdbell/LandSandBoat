require('scripts/globals/player')

describe('Player game-in lifecycle tail', function()
    it('records zone-in state, schedules completion, and clears gameLogin', function()
        local writes = {}
        local delay
        local callback
        local campaignCalls = 0
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        local originalCampaignOnGameIn = xi.events.loginCampaign.onGameIn
        xi.gear_sets.checkForGearSet = function() end
        xi.events.loginCampaign.onGameIn = function() campaignCalls = campaignCalls + 1 end

        local player = {
            getZoneID = function() return 100 end,
            getCharVarsWithSuffix = function() return {} end,
            getQuestStatus = function() return -1 end,
            hasCompletedQuest = function() return false end,
            getCharVar = function() return 0 end,
            setLocalVar = function(_, name, value) table.insert(writes, { name, value }) end,
            timer = function(_, milliseconds, fn)
                delay = milliseconds
                callback = fn
            end,
        }

        xi.player.onGameIn(player, false, false)
        assert(delay == 2500 and callback ~= nil)
        assert(#writes == 3)
        assert(writes[1][1] == 'ZoneInTime' and type(writes[1][2]) == 'number')
        assert(writes[2][1] == 'ZoningIn' and writes[2][2] == 1)
        assert(writes[3][1] == 'gameLogin' and writes[3][2] == 0)
        assert(campaignCalls == 0)

        callback(player)
        xi.gear_sets.checkForGearSet = originalCheckForGearSet
        xi.events.loginCampaign.onGameIn = originalCampaignOnGameIn
        assert(#writes == 4)
        assert(writes[4][1] == 'ZoningIn' and writes[4][2] == 0)
        assert(campaignCalls == 1)
    end)
end)
