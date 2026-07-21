-----------------------------------
-- Pure system tests for instance time message / fail plans (slice 6215).
-----------------------------------

describe('instance time pure plan', function()
    local function nextTimeMessage(last, remaining)
        if last == 0 and remaining < 600 then return 600 end
        if last == 600 and remaining < 300 then return 300 end
        if last == 300 and remaining < 60 then return 60 end
        if last == 60 and remaining < 30 then return 30 end
        if last == 30 and remaining < 10 then return 10 end
        return 0
    end

    local function messageArg(msg)
        if msg >= 60 then return msg / 60 end
        return msg
    end

    local function remainingSec(limitMin, elapsedMs)
        return limitMin * 60 - (elapsedMs / 1000)
    end

    local function shouldFail(remaining, wipeTime, elapsedMs)
        if remaining < 0 then return true end
        if wipeTime ~= 0 and (elapsedMs - wipeTime) / 1000 > 180 then return true end
        return false
    end

    local function allWiped(hps)
        for _, hp in ipairs(hps) do
            if hp ~= 0 then return false end
        end
        return true
    end

    it('time message thresholds cascade', function()
        assert(nextTimeMessage(0, 599) == 600)
        assert(nextTimeMessage(0, 600) == 0)
        assert(nextTimeMessage(600, 299) == 300)
        assert(nextTimeMessage(300, 59) == 60)
        assert(nextTimeMessage(60, 29) == 30)
        assert(nextTimeMessage(30, 9) == 10)
    end)

    it('message arg minutes vs seconds', function()
        assert(messageArg(600) == 10 and messageArg(30) == 30)
    end)

    it('remaining and fail pure', function()
        assert(remainingSec(30, 5 * 60 * 1000) == 1500)
        assert(shouldFail(-0.1, 0, 0))
        assert(shouldFail(100, 1000, 181001))
        assert(not shouldFail(100, 1000, 180000))
        assert(not shouldFail(100, 0, 999999))
    end)

    it('all players wiped scan', function()
        assert(allWiped({}))
        assert(allWiped({ 0, 0 }))
        assert(not allWiped({ 0, 1 }))
    end)
end)
