-----------------------------------
-- Pure system tests for HELM pickItem / onTrade plan (slice 6148).
-----------------------------------

describe('helm trade pure plan', function()
    local RED_ROCK = 769 -- placeholder colored rock pin (sample)

    local function rateSuccess(roll, settingRate)
        return roll <= settingRate
    end

    local function totalWeight(drops)
        local sum = 0
        for _, d in ipairs(drops) do
            sum = sum + d[1]
        end
        return sum
    end

    local function pickItem(roll, drops)
        local sum = 0
        local total = totalWeight(drops)
        if total <= 0 then
            return 0
        end
        for _, d in ipairs(drops) do
            sum = sum + d[1]
            if sum >= roll then
                return d[2]
            end
        end
        return 0
    end

    local function resolveRock(itemId, dayElement, rocks)
        if itemId ~= RED_ROCK then
            return itemId
        end
        return rocks[dayElement] or itemId
    end

    local function planPick(rateRoll, settingRate, weightRoll, drops, dayElement, rocks)
        if not rateSuccess(rateRoll, settingRate) then
            return 0
        end
        local item = pickItem(weightRoll, drops)
        return resolveRock(item, dayElement, rocks)
    end

    local function breaks(roll, settingBreak, modValue)
        return (roll + modValue / 10) <= settingBreak
    end

    local function tradeValid(hasTool, itemCount)
        return hasTool and itemCount == 1
    end

    -- full inventory zeroes reward after pick
    local function planTrade(p)
        if not tradeValid(p.hasTool, p.itemCount) then
            return { valid = false }
        end
        local itemId = planPick(p.rateRoll, p.settingRate, p.weightRoll, p.drops, p.dayElement, p.rocks)
        local broke = breaks(p.breakRoll, p.settingBreak, p.modValue) and 1 or 0
        local full = p.freeSlots == 0 and 1 or 0
        if full == 1 then
            itemId = 0
        end
        local uses = (p.uses - 1) % 4
        return {
            valid    = true,
            itemId   = itemId,
            broke    = broke,
            full     = full,
            uses     = uses,
            movePoint = (itemId ~= 0 and uses == 0),
            reward   = itemId ~= 0,
        }
    end

    local drops = {
        { 50, 100 },
        { 30, RED_ROCK },
        { 20, 200 },
    }
    local rocks = { [1] = 1111, [2] = 2222 }

    it('pick item rate gate and weighted select', function()
        assert(planPick(51, 50, 1, drops, 1, rocks) == 0)
        assert(planPick(50, 50, 50, drops, 1, rocks) == 100)
        -- weight roll 51..80 → RED_ROCK remapped
        assert(planPick(1, 50, 60, drops, 1, rocks) == 1111)
        assert(planPick(1, 50, 60, drops, 2, rocks) == 2222)
        -- non-rock passes through
        assert(planPick(1, 50, 90, drops, 1, rocks) == 200)
    end)

    it('trade invalid without single tool', function()
        local r = planTrade({
            hasTool = false, itemCount = 1,
            rateRoll = 1, settingRate = 50, weightRoll = 1, drops = drops,
            dayElement = 1, rocks = rocks,
            breakRoll = 1, settingBreak = 33, modValue = 0,
            freeSlots = 5, uses = 1,
        })
        assert(r.valid == false)
        r = planTrade({
            hasTool = true, itemCount = 2,
            rateRoll = 1, settingRate = 50, weightRoll = 1, drops = drops,
            dayElement = 1, rocks = rocks,
            breakRoll = 1, settingBreak = 33, modValue = 0,
            freeSlots = 5, uses = 1,
        })
        assert(r.valid == false)
    end)

    it('trade success reward and uses wrap', function()
        local r = planTrade({
            hasTool = true, itemCount = 1,
            rateRoll = 1, settingRate = 50, weightRoll = 50, drops = drops,
            dayElement = 1, rocks = rocks,
            breakRoll = 50, settingBreak = 33, modValue = 0, -- no break
            freeSlots = 5, uses = 1,
        })
        assert(r.valid and r.itemId == 100 and r.broke == 0 and r.reward)
        assert(r.uses == 0 and r.movePoint == true)
    end)

    it('full inventory zeroes item', function()
        local r = planTrade({
            hasTool = true, itemCount = 1,
            rateRoll = 1, settingRate = 50, weightRoll = 50, drops = drops,
            dayElement = 1, rocks = rocks,
            breakRoll = 1, settingBreak = 33, modValue = 0,
            freeSlots = 0, uses = 3,
        })
        assert(r.full == 1 and r.itemId == 0 and not r.reward and not r.movePoint)
        assert(r.uses == 2)
    end)

    it('tool break with mod mitigation', function()
        assert(breaks(33, 33, 0) == true)
        assert(breaks(34, 33, 0) == false)
        -- mod 20 → roll 32 + 2 = 34 > 33
        assert(breaks(32, 33, 20) == false)
        assert(breaks(31, 33, 20) == true)
    end)
end)
