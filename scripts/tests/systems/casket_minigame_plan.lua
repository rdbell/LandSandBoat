-----------------------------------
-- Pure system tests for casket minigame pure plans (slice 6187).
-----------------------------------

describe('casket minigame pure plan', function()
    local function convertTime(rawSeconds)
        local h = math.floor(rawSeconds / 3600)
        local m = math.floor(rawSeconds / 60 - h * 60)
        local s = math.floor(rawSeconds - h * 3600 - m * 60)
        return h, m, s
    end

    -- lastSpawned seconds → true when >= 1h or (>= 0h and >= 5m)
    local function timeElapsedOk(lastSpawned)
        local h, m = convertTime(lastSpawned)
        if h >= 1 then
            return true
        end
        if h < 1 and m >= 5 then
            return true
        end
        return false
    end

    local function remainingAttempts(allowed, failed)
        return allowed - failed
    end

    -- remaining == 1 after a fail path → reveal correct + remove
    local function planLockExhausted(remaining)
        return remaining == 1
    end

    -- option parse (32-bit shift mask like bit.lshift)
    local function parseLockOption(option)
        local shift = (option - 1) % 32
        local lockedChoice = bit.lshift(1, shift)
        local inputNumber = bit.rshift(option, 16)
        return lockedChoice, inputNumber
    end

    local function splitTwoDigits(n)
        local s = tostring(n)
        local digits = {}
        for d in string.gmatch(s, '%d') do
            table.insert(digits, tonumber(d))
        end
        return digits
    end

    -- remove one digit char from hints table string
    local function removeHint(hintVar, hintNum)
        local hintString = tostring(hintVar)
        local newHintString
        if #hintString > 1 then
            newHintString = hintString:gsub(tostring(hintNum), '')
        else
            newHintString = '0'
        end
        return tonumber(newHintString)
    end

    -- thief tools range pure (rolls are 1..9 injects)
    local function planThiefRange(firstDigit, lowRoll, highRoll)
        if firstDigit == 1 then
            return 10, 20 + highRoll
        elseif firstDigit > 1 and firstDigit < 9 then
            return firstDigit * 10 - 10 + lowRoll, firstDigit * 10 + 10 + highRoll
        elseif firstDigit == 9 then
            return 80 + lowRoll, 99
        end
        return 0, 0
    end

    it('convertTime splits h/m/s', function()
        local h, m, s = convertTime(0)
        assert(h == 0 and m == 0 and s == 0)
        h, m, s = convertTime(3661)
        assert(h == 1 and m == 1 and s == 1)
        h, m, s = convertTime(300)
        assert(h == 0 and m == 5 and s == 0)
    end)

    it('timeElapsedOk five-minute gate', function()
        assert(not timeElapsedOk(0))
        assert(not timeElapsedOk(299))
        assert(timeElapsedOk(300))
        assert(timeElapsedOk(3600))
        assert(timeElapsedOk(7200))
    end)

    it('remaining and exhaust', function()
        assert(remainingAttempts(5, 2) == 3)
        assert(planLockExhausted(1))
        assert(not planLockExhausted(2))
    end)

    it('parses lock option', function()
        -- option 258 → examine (lockedChoice 2), input 0
        local choice, num = parseLockOption(258)
        assert(choice == 2 and num == 0)
        -- guess 45: option = 45<<16 | low? rshift 16 = 45
        choice, num = parseLockOption(bit.lshift(45, 16) + 1)
        assert(num == 45)
    end)

    it('splits digits and removes hint', function()
        local d = splitTwoDigits(47)
        assert(d[1] == 4 and d[2] == 7)
        assert(removeHint(1234, 2) == 134)
        assert(removeHint(5, 5) == 0)
    end)

    it('thief tools range bounds', function()
        local lo, hi = planThiefRange(1, 1, 3)
        assert(lo == 10 and hi == 23)
        lo, hi = planThiefRange(5, 2, 4)
        assert(lo == 42 and hi == 64) -- 50-10+2, 50+10+4
        lo, hi = planThiefRange(9, 7, 1)
        assert(lo == 87 and hi == 99)
    end)
end)
