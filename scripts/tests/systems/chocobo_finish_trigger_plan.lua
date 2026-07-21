-----------------------------------
-- Pure system tests for chocobo finish/trigger pure plans (slice 6172).
-----------------------------------

describe('chocobo finish and trigger pure plan', function()
    local TRADE = 100
    local MAIN = 200
    local REMINDER = 50

    -- onEventFinish pure routing
    local FINISH_CREATE = 1
    local FINISH_RETIRE = 2
    local FINISH_UPDATE = 3
    local FINISH_NONE = 0

    local function planFinish(csid, option, tradeCsid, mainCsid, hasState, retiring)
        if csid == tradeCsid and option == 252 then
            return FINISH_CREATE
        end
        if csid == mainCsid then
            if not hasState then
                return FINISH_NONE
            end
            if retiring then
                return FINISH_RETIRE
            end
            return FINISH_UPDATE
        end
        return FINISH_NONE
    end

    -- onTrigger pure: no state / wrong loc → reminder; else main with infoFlag
    local function planTrigger(hasState, stateLoc, zoneLoc, reportCount)
        if not hasState then
            return { reminder = true, reminderArgs = { 1 } }
        end
        if stateLoc ~= zoneLoc then
            return { reminder = true, reminderArgs = { 1, 1, 1, 1 } }
        end
        local infoFlag = 0
        if reportCount > 0 then
            infoFlag = 1
        end
        return { main = true, infoFlag = infoFlag, isTradeEvent = 0 }
    end

    -- trade finish option 252 on trade csid → location updateEvent arg
    local function planTradeUpdate(option, raisingLocation)
        if option == 252 then
            return { ok = true, location = raisingLocation }
        end
        return { ok = false }
    end

    it('finish creates on trade option 252', function()
        assert(planFinish(TRADE, 252, TRADE, MAIN, false, false) == FINISH_CREATE)
        assert(planFinish(TRADE, 0, TRADE, MAIN, false, false) == FINISH_NONE)
    end)

    it('finish retires or updates on main', function()
        assert(planFinish(MAIN, 0, TRADE, MAIN, true, true) == FINISH_RETIRE)
        assert(planFinish(MAIN, 0, TRADE, MAIN, true, false) == FINISH_UPDATE)
        assert(planFinish(MAIN, 215, TRADE, MAIN, false, false) == FINISH_NONE)
    end)

    it('trigger routes reminder and main', function()
        local r = planTrigger(false, 0, 1, 0)
        assert(r.reminder and r.reminderArgs[1] == 1)
        r = planTrigger(true, 1, 2, 0)
        assert(r.reminder and #r.reminderArgs == 4)
        r = planTrigger(true, 1, 1, 3)
        assert(r.main and r.infoFlag == 1 and r.isTradeEvent == 0)
        r = planTrigger(true, 1, 1, 0)
        assert(r.main and r.infoFlag == 0)
    end)

    it('trade cs update for option 252', function()
        local r = planTradeUpdate(252, 2)
        assert(r.ok and r.location == 2)
        assert(not planTradeUpdate(0, 2).ok)
    end)
end)
