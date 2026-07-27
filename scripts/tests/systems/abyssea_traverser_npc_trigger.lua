require('scripts/globals/abyssea')

describe('Abyssea traverser NPC trigger', function()
    local function player(zone, truth, dawn, available, held, cap)
        local events = {}
        local messages = 0
        return {
            getZoneID = function() return zone end,
            getAvailableTraverserStones = function() return available end,
            getQuestStatus = function() return dawn and xi.questStatus.QUEST_ACCEPTED or 0 end,
            hasCompletedQuest = function() return truth end,
            startEvent = function(_, ...) table.insert(events, { ... }) end,
            messageText = function() messages = messages + 1 end,
            events = function() return events end,
            messages = function() return messages end,
            held = function() return held end,
            cap = function() return cap end,
        }
    end

    local oldHeld = xi.abyssea.getHeldTraverserStones
    local oldCap = xi.abyssea.getTraverserCap

    before_each(function()
        xi.abyssea.getHeldTraverserStones = function(p) return p.held() end
        xi.abyssea.getTraverserCap = function(p) return p.cap() end
    end)

    after_each(function()
        xi.abyssea.getHeldTraverserStones = oldHeld
        xi.abyssea.getTraverserCap = oldCap
    end)

    it('rejects unacquainted players outside Port Jeuno', function()
        local p = player(xi.zone.PORT_BASTOK, false, true, 1, 0, 3)
        xi.abyssea.traverserNPCOnTrigger(p)
        assert(p.messages() == 1 and #p.events() == 0)
    end)

    it('opens the menu in Port Jeuno before The Truth Beckons', function()
        local p = player(xi.zone.PORT_JEUNO, false, true, 2, 1, 3)
        xi.abyssea.traverserNPCOnTrigger(p)
        assert(p.events()[1][1] == 328 and p.events()[1][3] == 2 and p.events()[1][4] == 1 and p.events()[1][5] == 0)
    end)

    it('reports maximum held stones ahead of availability', function()
        local p = player(xi.zone.PORT_BASTOK, true, true, 2, 3, 3)
        xi.abyssea.traverserNPCOnTrigger(p)
        assert(p.events()[1][5] == 1)
    end)
end)
