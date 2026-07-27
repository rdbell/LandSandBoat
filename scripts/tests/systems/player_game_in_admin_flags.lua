require('scripts/globals/player')

describe('Player game-in administrative flags', function()
    it('enables Immortal and GM-hidden independently', function()
        local immortal = false
        local hidden = false
        local unkillableCalls = 0
        local hiddenCalls = 0
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        xi.gear_sets.checkForGearSet = function() end

        local player = {
            getZoneID = function() return 100 end,
            getCharVarsWithSuffix = function() return {} end,
            getQuestStatus = function() return -1 end,
            hasCompletedQuest = function() return false end,
            getCharVar = function(_, name)
                if name == 'Immortal' then
                    return immortal and 1 or 0
                end
                if name == 'GMHidden' then
                    return hidden and 1 or 0
                end
                return 0
            end,
            setUnkillable = function(_, enabled)
                assert(enabled)
                unkillableCalls = unkillableCalls + 1
            end,
            setGMHidden = function(_, enabled)
                assert(enabled)
                hiddenCalls = hiddenCalls + 1
            end,
            setLocalVar = function() end,
            timer = function() end,
        }

        xi.player.onGameIn(player, false, false)
        assert(unkillableCalls == 0 and hiddenCalls == 0)

        immortal = true
        xi.player.onGameIn(player, false, false)
        assert(unkillableCalls == 1 and hiddenCalls == 0)

        hidden = true
        xi.player.onGameIn(player, false, false)
        xi.gear_sets.checkForGearSet = originalCheckForGearSet
        assert(unkillableCalls == 2 and hiddenCalls == 1)
    end)
end)
