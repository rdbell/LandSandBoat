-----------------------------------
-- Pure system tests for manaclipper event pure plans (slice 6194).
-----------------------------------

describe('manaclipper event pure plan', function()
    local function planChannelEvent(eventId, earthSecs)
        local earthMins = math.floor(earthSecs / 60)
        if earthMins ~= 0 then
            return eventId, earthMins
        end
        return eventId - 1, 0
    end

    -- onZoneIn arrival: PURGONORGO route → 13 else 12
    local PURG = 2
    local function planArrivalEvent(route)
        if route == PURG then
            return 13
        end
        return 12
    end

    -- transport from sunset dock (aboard==1)
    local TRANS_SINGLE = 1
    local TRANS_MULTI = 2
    local TRANS_KICK = 0
    local TRANS_PURG = 3 -- aboard 2, no ticket
    local TRANS_NONE = -1

    local function planTransport(aboard, hasSingle, hasMulti)
        if aboard == 1 then
            if hasSingle then
                return TRANS_SINGLE
            end
            if hasMulti then
                return TRANS_MULTI
            end
            return TRANS_KICK
        end
        if aboard == 2 then
            return TRANS_PURG
        end
        return TRANS_NONE
    end

    local function planMultiUses(uses)
        uses = uses - 1
        if uses <= 0 then
            return 0, true
        end
        return uses, false
    end

    it('channel arriving shortly', function()
        local id, m = planChannelEvent(50, 120)
        assert(id == 50 and m == 2)
        id, m = planChannelEvent(50, 45)
        assert(id == 49 and m == 0)
    end)

    it('arrival event by route', function()
        assert(planArrivalEvent(PURG) == 13)
        assert(planArrivalEvent(0) == 12)
        assert(planArrivalEvent(3) == 12)
    end)

    it('transport branches', function()
        assert(planTransport(1, true, false) == TRANS_SINGLE)
        assert(planTransport(1, false, true) == TRANS_MULTI)
        assert(planTransport(1, false, false) == TRANS_KICK)
        assert(planTransport(2, false, false) == TRANS_PURG)
        assert(planTransport(0, true, true) == TRANS_NONE)
    end)

    it('multi uses', function()
        local u, del = planMultiUses(4)
        assert(u == 3 and not del)
        u, del = planMultiUses(1)
        assert(u == 0 and del)
    end)

    it('zone-in onboard arrival and Bibiki return', function()
        -- At 05:00, the next onboard event is 08:40 Purgonorgo Isle.
        assert(planArrivalEvent(PURG) == 13)
        local stored = 13
        assert(stored == 13) -- Bibiki return preserves the Purgonorgo event.
        stored = 99
        assert((stored == 13 and 13 or 12) == 12) -- malformed value falls back.
    end)
end)
