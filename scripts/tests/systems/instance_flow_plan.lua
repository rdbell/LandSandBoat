-----------------------------------
-- Pure system tests for instance onTrigger/onEventUpdate plans (slice 6216).
-----------------------------------

describe('instance flow pure plan', function()
    local QUEST_AVAILABLE, QUEST_ACCEPTED, QUEST_COMPLETED = 0, 1, 2
    local WAKING = 7702

    local function instanceName(instanceId, divineStatus)
        if instanceId ~= WAKING then return nil end
        if divineStatus >= QUEST_ACCEPTED then return 1 end
        return 0
    end

    local function pickEntry(entries, registryOK)
        for _, e in ipairs(entries) do
            if registryOK(e[1]) then return e end
        end
        return nil
    end

    local function partyGate(members)
        for _, m in ipairs(members) do
            if not m.isSelf and m.sameZone then
                if not m.entryOK then return 'no_reqs' end
                if m.distance > 50 then return 'too_far' end
            end
        end
        return 'ok'
    end

    local function shouldWait(hasInstance, requested)
        if hasInstance then return true end
        return requested > 0 and requested < 10
    end

    it('getInstanceName for 7702 divine interference', function()
        assert(instanceName(WAKING, QUEST_ACCEPTED) == 1)
        assert(instanceName(WAKING, QUEST_COMPLETED) == 1)
        assert(instanceName(WAKING, QUEST_AVAILABLE) == 0)
        assert(instanceName(7700, QUEST_ACCEPTED) == nil)
    end)

    it('pick first registry-valid lookup entry', function()
        local entries = { { 5600 }, { 5601 } }
        local e = pickEntry(entries, function(id) return id == 5601 end)
        assert(e and e[1] == 5601)
        assert(pickEntry(entries, function() return false end) == nil)
    end)

    it('party entry gates: reqs and distance 50', function()
        assert(partyGate({ { isSelf = true, sameZone = true } }) == 'ok')
        assert(partyGate({ { sameZone = true, entryOK = true, distance = 10 } }) == 'ok')
        assert(partyGate({ { sameZone = true, entryOK = false, distance = 1 } }) == 'no_reqs')
        assert(partyGate({ { sameZone = true, entryOK = true, distance = 50.1 } }) == 'too_far')
        assert(partyGate({ { sameZone = false, entryOK = false, distance = 999 } }) == 'ok')
    end)

    it('INSTANCE_REQUESTED wait window', function()
        assert(shouldWait(true, 0))
        assert(shouldWait(false, 1) and shouldWait(false, 9))
        assert(not shouldWait(false, 0) and not shouldWait(false, 10))
    end)
end)
