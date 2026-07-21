-----------------------------------
-- Pure system tests for dealer moogle trade/finish gates (slice 6179).
-----------------------------------

describe('dealer moogle event pure plan', function()
    local EVENT_NONE = 0
    local EVENT_TRIGGER = 1
    local EVENT_TRADE = 2

    -- Trade accept: has coupon item and missing KI
    local function planTradeAccept(hasItem, hasKI)
        if hasItem and not hasKI then
            return { accept = true, eventType = EVENT_TRADE }
        end
        return { accept = false }
    end

    -- Trigger CS: amount > 0 → base+1 and EventTrigger
    local function planTriggerCS(baseCS, amount)
        if amount > 0 then
            return { csid = baseCS + 1, eventType = EVENT_TRIGGER }
        end
        return { csid = baseCS, eventType = EVENT_NONE }
    end

    -- Finish early exits
    local function planFinishGate(csid, expectedCS, option, eventType, obtainKI, requiredKI)
        if csid ~= expectedCS then
            return { action = 'none', reason = 'csid' }
        end
        if option == 0 then
            return { action = 'none', reason = 'option' }
        end
        if requiredKI == 0 then
            return { action = 'none', reason = 'nokey' }
        end
        -- option 1..255 trade path: give KI already done; just return
        if option <= 255 and eventType == EVENT_TRADE and obtainKI == requiredKI then
            return { action = 'trade_ki_done' }
        end
        -- must have KI (trigger) or matching obtainKI (trade)
        if eventType == EVENT_TRIGGER and not true then -- hasKI inject below
        end
        return { action = 'continue', requiredKI = requiredKI }
    end

    local function planHasAccess(eventType, hasKI, obtainKI, requiredKI)
        if eventType == EVENT_TRIGGER then
            return hasKI
        end
        if eventType == EVENT_TRADE then
            return obtainKI == requiredKI
        end
        return false
    end

    -- list 19 vs item grant routing
    local function planGrantRoute(itemList, idx, keyItems)
        if itemList == 19 and idx > 0 and keyItems > 0 then
            return 'keyitem_list'
        end
        if itemList > 0 and idx > 0 and keyItems == 0 then
            return 'item_list'
        end
        return 'none'
    end

    it('accepts trade only with item and without KI', function()
        assert(planTradeAccept(true, false).accept and planTradeAccept(true, false).eventType == EVENT_TRADE)
        assert(not planTradeAccept(false, false).accept)
        assert(not planTradeAccept(true, true).accept)
    end)

    it('shifts trigger cs when amount > 0', function()
        local r = planTriggerCS(100, 2)
        assert(r.csid == 101 and r.eventType == EVENT_TRIGGER)
        r = planTriggerCS(100, 0)
        assert(r.csid == 100 and r.eventType == EVENT_NONE)
    end)

    it('finish gates on csid option and keys', function()
        assert(planFinishGate(1, 2, 1, EVENT_TRADE, 5, 5).reason == 'csid')
        assert(planFinishGate(2, 2, 0, EVENT_TRADE, 5, 5).reason == 'option')
        assert(planFinishGate(2, 2, 1, EVENT_TRADE, 5, 0).reason == 'nokey')
        assert(planFinishGate(2, 2, 10, EVENT_TRADE, 5, 5).action == 'trade_ki_done')
        assert(planFinishGate(2, 2, 300, EVENT_TRIGGER, 0, 5).action == 'continue')
    end)

    it('access check for trigger vs trade', function()
        assert(planHasAccess(EVENT_TRIGGER, true, 0, 5))
        assert(not planHasAccess(EVENT_TRIGGER, false, 0, 5))
        assert(planHasAccess(EVENT_TRADE, false, 5, 5))
        assert(not planHasAccess(EVENT_TRADE, false, 4, 5))
    end)

    it('routes grant by list type', function()
        assert(planGrantRoute(19, 1, 3) == 'keyitem_list')
        assert(planGrantRoute(5, 2, 0) == 'item_list')
        assert(planGrantRoute(19, 0, 3) == 'none')
        assert(planGrantRoute(5, 2, 1) == 'none')
    end)
end)
