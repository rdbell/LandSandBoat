-----------------------------------
-- Pure system tests for gobbie mystery box event plans (slice 6178).
-----------------------------------

describe('gobbie mystery box event pure plan', function()
    local MIN_AGE = 45
    local costs = { [1] = 10, [2] = 10, [3] = 10, [4] = 10, [5] = 10, [6] = 50 }

    local function planTrade(itemCount, tradeID, freeSlots, keyToDial)
        if itemCount ~= 1 then
            return { ok = false }
        end
        local dial = keyToDial[tradeID]
        if dial == nil then
            return { ok = false } -- points trade TODO
        end
        if freeSlots == 0 then
            return { ok = true, fullInv = true, tradeID = tradeID, dial = dial }
        end
        return { ok = true, keyTrade = true, tradeID = tradeID, dial = dial }
    end

    local function dialUsedBits(gobbieBoxUsed)
        return {
            special = bit.band(gobbieBoxUsed, 1) ~= 0 and 1 or 0,
            adoulin = bit.band(bit.rshift(gobbieBoxUsed, 1), 1) ~= 0 and 1 or 0,
            pictlogica = bit.band(bit.rshift(gobbieBoxUsed, 2), 1) ~= 0 and 1 or 0,
            wanted = bit.band(bit.rshift(gobbieBoxUsed, 3), 1) ~= 0 and 1 or 0,
        }
    end

    local function planTrigger(playerAgeDays, dailyTally, holdingItem)
        local firstVisit = dailyTally == -1
        if playerAgeDays >= MIN_AGE and firstVisit then
            return { intro = true }
        elseif playerAgeDays >= MIN_AGE then
            if holdingItem ~= 0 then
                return { holding = true }
            end
            return { default = true }
        else
            local waitDays = MIN_AGE - playerAgeDays + 1
            return { tooYoung = true, waitDays = waitDays }
        end
    end

    local function parseDialOption(option)
        return math.floor(option / 8), option % 8
    end

    -- dial spin optionType 1
    local function planDialSpin(dial, dialUsed, points, cost)
        if dialUsed then
            return { result = 'used', dial = dial }
        end
        if points >= cost then
            return { result = 'ok', dial = dial, newPoints = points - cost, markDial = dial >= 6 }
        end
        return { result = 'nopoints', dial = dial }
    end

    local function planAnniversary(roll, exclusivePick, dailyItem)
        if roll == 1 then
            return exclusivePick
        end
        return dailyItem
    end

    local function planAbjuration(picked, alreadyHas, junkPick)
        if alreadyHas then
            return junkPick
        end
        return picked
    end

    local function planFinish(csid, introCsid, holdingCsid, freeSlots, holdingItem)
        if csid == introCsid then
            return { setTally = 50 }
        end
        if csid == holdingCsid then
            if freeSlots == 0 then
                return { invFull = true }
            end
            if holdingItem > 0 then
                return { giveItem = holdingItem, clearHolding = true }
            end
        end
        return {}
    end

    it('routes key trade and full inv', function()
        local k2d = { [8973] = 6 }
        local r = planTrade(1, 8973, 5, k2d)
        assert(r.keyTrade and r.dial == 6)
        r = planTrade(1, 8973, 0, k2d)
        assert(r.fullInv)
        r = planTrade(2, 8973, 5, k2d)
        assert(not r.ok)
        r = planTrade(1, 1, 5, k2d)
        assert(not r.ok)
    end)

    it('trigger age and holding branches', function()
        assert(planTrigger(50, -1, 0).intro)
        assert(planTrigger(50, 100, 123).holding)
        assert(planTrigger(50, 100, 0).default)
        local r = planTrigger(10, 0, 0)
        assert(r.tooYoung and r.waitDays == 45 - 10 + 1)
    end)

    it('parses dial used bits', function()
        local b = dialUsedBits(0)
        assert(b.special == 0 and b.wanted == 0)
        b = dialUsedBits(bit.bor(1, bit.lshift(1, 3)))
        assert(b.special == 1 and b.wanted == 1 and b.adoulin == 0)
    end)

    it('parses option into dial and type', function()
        local dial, typ = parseDialOption(8 + 1) -- dial 1, type 1
        assert(dial == 1 and typ == 1)
        dial, typ = parseDialOption(6 * 8 + 5)
        assert(dial == 6 and typ == 5)
    end)

    it('dial spin outcomes', function()
        assert(planDialSpin(1, true, 100, 10).result == 'used')
        local r = planDialSpin(1, false, 100, 10)
        assert(r.result == 'ok' and r.newPoints == 90 and not r.markDial)
        r = planDialSpin(6, false, 50, 50)
        assert(r.result == 'ok' and r.markDial)
        assert(planDialSpin(1, false, 5, 10).result == 'nopoints')
    end)

    it('anniversary and abjuration picks', function()
        assert(planAnniversary(1, 999, 1) == 999)
        assert(planAnniversary(2, 999, 1) == 1)
        assert(planAbjuration(50, true, 7) == 7)
        assert(planAbjuration(50, false, 7) == 50)
    end)

    it('finish intro and holding', function()
        assert(planFinish(1, 1, 2, 5, 0).setTally == 50)
        assert(planFinish(2, 1, 2, 0, 10).invFull)
        local r = planFinish(2, 1, 2, 3, 10)
        assert(r.giveItem == 10 and r.clearHolding)
    end)
end)
