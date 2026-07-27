require('scripts/globals/abyssea')

describe('Abyssea zone key-item reward', function()
    it('returns Lunar Abyssite 2 before completing maw quests', function()
        local p = { hasCompletedQuest = function() return false end }
        assert(xi.abyssea.getZoneKIReward(p) == xi.ki.LUNAR_ABYSSITE2)
    end)

    it('returns Ivory Abyssite of Destiny after eight maw quests', function()
        local completed = 0
        local p = { hasCompletedQuest = function() completed = completed + 1 return completed <= 8 end }
        assert(xi.abyssea.getZoneKIReward(p) == xi.ki.IVORY_ABYSSITE_OF_DESTINY)
    end)

    it('returns nil after all nine maw quests', function()
        local p = { hasCompletedQuest = function() return true end }
        assert(xi.abyssea.getZoneKIReward(p) == nil)
    end)
end)
