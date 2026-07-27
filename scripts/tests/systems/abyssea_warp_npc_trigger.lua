require('scripts/globals/abyssea')

describe('Abyssea warp NPC trigger', function()
    local function player(unlocked, truth)
        local event
        return {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCurrency = function() return 1000 end,
            getQuestStatus = function() return unlocked and xi.questStatus.QUEST_ACCEPTED or 0 end,
            hasCompletedQuest = function() return truth end,
            startEvent = function(_, ...) event = { ... } end,
            event = function() return event end,
        }
    end

    it('packs all unlocked maws into the warp menu', function()
        local p = player(true, true)
        xi.abyssea.warpNPCOnTrigger(p)
        assert(p.event()[1] == 404 and p.event()[2] == 2 and p.event()[3] == 1000)
        assert(p.event()[4] == 7 and p.event()[5] == 7 and p.event()[6] == 7)
    end)

    it('leaves locked maws and the truth flag clear', function()
        local p = player(false, false)
        xi.abyssea.warpNPCOnTrigger(p)
        assert(p.event()[2] == 0 and p.event()[4] == 0 and p.event()[5] == 0 and p.event()[6] == 0)
    end)
end)
