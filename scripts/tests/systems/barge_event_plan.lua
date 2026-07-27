-----------------------------------
-- Pure system tests for barge event pure plans (slice 6193).
-----------------------------------

describe('barge event pure plan', function()
    local function remainingGameMins(currentTime, endTime)
        local gameMins = endTime - currentTime
        if endTime < currentTime then
            gameMins = 1440 + endTime - currentTime
        end
        return gameMins
    end

    local function earthSecs(gameMins)
        return math.floor(gameMins * 60 / 25)
    end

    -- channel: earthMins==0 → eventId-1 (arriving shortly)
    local function planChannelEvent(eventId, earthSecsVal)
        local earthMins = math.floor(earthSecsVal / 60)
        if earthMins ~= 0 then
            return eventId, earthMins
        end
        return eventId - 1, 0
    end

    local TRANS_KICK = 0
    local TRANS_SINGLE = 1
    local TRANS_MULTI = 2
    local TRANS_NONE = -1

    local function planTransport(inTrigger, transportValid, hasSingle, hasMulti, multiUses)
        if not inTrigger then
            return TRANS_NONE
        end
        if not transportValid then
            return TRANS_KICK
        end
        if hasSingle then
            return TRANS_SINGLE
        end
        if hasMulti then
            return TRANS_MULTI
        end
        return TRANS_KICK
    end

    local function planMultiUses(usesLeft)
        usesLeft = usesLeft - 1
        if usesLeft <= 0 then
            return 0, true -- del KI
        end
        return usesLeft, false
    end

    -- hasKeyItemParam: bit i-1 for tickets[i]
    local function planTicketHasMask(hasSingle, hasMulti)
        local m = 0
        if hasSingle then
            m = bit.bor(m, 1) -- bit 0
        end
        if hasMulti then
            m = bit.bor(m, 2) -- bit 1
        end
        return m
    end

    -- can buy: not has KI or (charges and numberTicket < charges)
    local function planCanBuy(hasKI, charges, numberTicket, gil, cost)
        if hasKI and not (charges and numberTicket < charges) then
            return false
        end
        return gil >= cost
    end

    local function planTicketCsidOk(csid)
        return csid == 31 or csid == 32 or csid == 43
    end

    local function nextScheduleEvent(currentTime, schedule)
        if schedule[#schedule].endTime <= currentTime or schedule[1].endTime > currentTime then
            return schedule[1]
        end

        for _, event in ipairs(schedule) do
            if event.endTime > currentTime then
                return event
            end
        end
    end

    local function nextChannelRoute(currentTime)
        local channel = {
            { endTime = 275, route = 1 }, { endTime = 535, route = 0 },
            { endTime = 960, route = 2 }, { endTime = 1155, route = 3 },
            { endTime = 1415, route = 0 },
        }
        return nextScheduleEvent(currentTime, channel).route
    end

    local function landingArrivalEvent(destination)
        local docks = { [0] = 38, [2] = 11, [3] = 10 }
        return docks[destination] or docks[3]
    end

    local function planTimekeeper(schedule, channel, currentTime, eventId)
        local next = nextScheduleEvent(currentTime, schedule)
        local gameMins = remainingGameMins(currentTime, next.endTime)
        local earthSecsVal = earthSecs(gameMins)

        if channel then
            local useEventId, earthMins = planChannelEvent(eventId, earthSecsVal)
            return useEventId, earthMins, math.floor(gameMins / 60), next.route
        end

        return eventId, earthSecsVal, next.action, 0, next.route
    end

    it('remaining and earth secs', function()
        assert(remainingGameMins(100, 160) == 60)
        assert(remainingGameMins(1400, 10) == 50) -- wrap
        assert(earthSecs(25) == 60)
    end)

    it('channel event id', function()
        local id, mins = planChannelEvent(100, 120) -- 2 earth mins
        assert(id == 100 and mins == 2)
        id, mins = planChannelEvent(100, 30) -- 0 earth mins
        assert(id == 99 and mins == 0)
    end)

    it('transport branches', function()
        assert(planTransport(false, true, true, false, 0) == TRANS_NONE)
        assert(planTransport(true, false, true, false, 0) == TRANS_KICK)
        assert(planTransport(true, true, true, false, 0) == TRANS_SINGLE)
        assert(planTransport(true, true, false, true, 5) == TRANS_MULTI)
        assert(planTransport(true, true, false, false, 0) == TRANS_KICK)
    end)

    it('multi uses and ticket buy', function()
        local u, del = planMultiUses(3)
        assert(u == 2 and not del)
        u, del = planMultiUses(1)
        assert(u == 0 and del)
        assert(planTicketHasMask(true, false) == 1)
        assert(planTicketHasMask(false, true) == 2)
        assert(planTicketHasMask(true, true) == 3)
        assert(planCanBuy(false, nil, 0, 50, 50))
        assert(not planCanBuy(true, nil, 0, 50, 50)) -- already has single
        assert(planCanBuy(true, 10, 5, 300, 300)) -- multi under max
        assert(not planCanBuy(true, 10, 10, 300, 300)) -- multi full
        assert(not planCanBuy(false, nil, 0, 49, 50)) -- poor
        assert(planTicketCsidOk(31) and planTicketCsidOk(43) and not planTicketCsidOk(1))
    end)

    it('channel route and landing fallback pins', function()
        -- At 08:55, the strict next-event lookup selects the 16:00 North route.
        assert(nextChannelRoute(535) == 2)
        assert(nextChannelRoute(1415) == 1) -- wraps to Central Landing Emfea
        assert(landingArrivalEvent(1) == 10) -- EMFEA falls back to Central
    end)

    it('timekeeper dock and channel event arguments', function()
        local central = {
            { endTime = 275, action = 0, route = 0 },
            { endTime = 325, action = 1, route = 0 },
            { endTime = 1155, action = 0, route = 0 },
            { endTime = 1205, action = 1, route = 0 },
        }
        local channel = {
            { endTime = 275, route = 1 }, { endTime = 535, route = 0 },
            { endTime = 960, route = 2 }, { endTime = 1155, route = 3 },
            { endTime = 1415, route = 0 },
        }

        local id, a, b, c, d = planTimekeeper(central, false, 300, 100)
        assert(id == 100 and a == 60 and b == 1 and c == 0 and d == 0)
        id, a, b, c = planTimekeeper(channel, true, 535, 100)
        assert(id == 100 and a == 17 and b == 7 and c == 2)
        id, a, b, c = planTimekeeper(channel, true, 1410, 100)
        assert(id == 99 and a == 0 and b == 0 and c == 0)
    end)
end)
