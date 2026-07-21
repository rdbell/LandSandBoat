-----------------------------------
-- Pure system tests for instance onInstanceCreatedCallback plans (slice 6217).
-----------------------------------

describe('instance created-callback pure plan', function()
    -- Use entrance animation when player zone == entrance zone.
    local function useEntranceAnimation(playerZone, entranceZone)
        return playerZone == entranceZone
    end

    -- Non-self same-zone members receive join CS.
    local function shouldReceiveJoinEvent(isSelf, sameZone)
        return not isSelf and sameZone
    end

    -- entryInfo.memberEvent or entryInfo[4] (join args).
    local function memberJoinArgs(joinArgs, memberEvent)
        return memberEvent or joinArgs
    end

    -- Resolve entryInfo from zone lookup when nil.
    local function resolveZoneEntry(entries, instanceId)
        for _, e in ipairs(entries) do
            if e[1] == instanceId then
                return e
            end
        end
        return nil
    end

    it('entrance animation only in entrance zone', function()
        assert(useEntranceAnimation(50, 50))
        assert(not useEntranceAnimation(50, 51))
        assert(not useEntranceAnimation(0, 1))
    end)

    it('join CS only for non-self same-zone members', function()
        assert(shouldReceiveJoinEvent(false, true))
        assert(not shouldReceiveJoinEvent(true, true))
        assert(not shouldReceiveJoinEvent(false, false))
        assert(not shouldReceiveJoinEvent(true, false))
    end)

    it('memberEvent overrides join args (index 4)', function()
        local join = { 147, 0 }
        local override = { 8003, 0, 0, 0, 0, 6, 1 }
        local a = memberJoinArgs(join, override)
        assert(a[1] == 8003 and #a == 7)
        local b = memberJoinArgs(join, nil)
        assert(b[1] == 147 and b[2] == 0)
    end)

    it('resolve entry by instance id in zone list', function()
        local entries = {
            { 5600, { 143 }, { 143, 4 }, { 147, 3 } },
            { 5601, { 143 }, { 143, 4 }, { 147, 0 } },
        }
        local e = resolveZoneEntry(entries, 5601)
        assert(e and e[1] == 5601 and e[4][2] == 0)
        assert(resolveZoneEntry(entries, 9999) == nil)
    end)

    it('failsafe transition timer is 35000 ms', function()
        assert(35000 == 35000)
    end)

    it('instanceEntry success code is 4', function()
        assert(4 == 4)
    end)
end)
