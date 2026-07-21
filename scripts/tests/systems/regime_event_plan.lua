-----------------------------------
-- Pure system tests for regime book event pure plans (slice 6181).
-----------------------------------

describe('regime event pure plan', function()
    local FIELDS = 1
    local GROUNDS = 2

    -- arg2: hide pages beyond catalog (bits 1..10 set when i > pages)
    local function planPageMask(pageCount)
        local arg2 = 0
        for i = 1, 10 do
            if i > pageCount then
                arg2 = arg2 + 2 ^ i
            end
        end
        return arg2
    end

    -- cipher: 3 when spring/fall or both, else 0
    local function planCipher(campaign)
        -- SPRING_FALL=1, BOTH=3 (example pins)
        if campaign == 1 or campaign == 3 then
            return 3
        end
        return 0
    end

    local function planArg4(hasRhapsodyWhite)
        return hasRhapsodyWhite and 1 or 0
    end

    -- trigger branch
    local BRANCH_HUNT = 1
    local BRANCH_BOOK = 2
    local BRANCH_DISABLED = 3

    local function planTrigger(huntStatus, regimeType, enableFields, enableGrounds)
        if huntStatus >= 1 then
            return BRANCH_HUNT
        end
        if regimeType == FIELDS and enableFields then
            return BRANCH_BOOK
        end
        if regimeType == GROUNDS and enableGrounds then
            return BRANCH_BOOK
        end
        return BRANCH_DISABLED
    end

    local function planFinishCost(cost, discounted, hasKI)
        if cost == nil then
            return nil
        end
        if hasKI and discounted ~= nil then
            return discounted
        end
        return cost
    end

    local function planCanAfford(cost, tabs)
        if cost == nil then
            return true
        end
        return cost <= tabs
    end

    local function planFoodBlocked(isFood, hasFoodEffect)
        return isFood and hasFoodEffect
    end

    local function planProtectTier(mLvl)
        if mLvl < 27 then
            return 20, 1
        elseif mLvl < 47 then
            return 50, 2
        elseif mLvl < 63 then
            return 90, 3
        elseif mLvl < 76 then
            return 140, 4
        end
        return 220, 5
    end

    local function planRepeatFlag(option)
        local regimeRepeat = bit.band(option, 0x80000000) ~= 0
        local stripped = bit.band(option, 0x7FFFFFFF)
        return regimeRepeat, stripped
    end

    it('builds page hide mask', function()
        assert(planPageMask(10) == 0)
        assert(planPageMask(0) == 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024)
        -- pageCount 8 → hide i=9,10 → 2^9 + 2^10
        assert(planPageMask(8) == 512 + 1024)
    end)

    it('cipher and rhapsody flags', function()
        assert(planCipher(1) == 3 and planCipher(3) == 3 and planCipher(0) == 0)
        assert(planArg4(true) == 1 and planArg4(false) == 0)
    end)

    it('trigger branches', function()
        assert(planTrigger(1, FIELDS, true, true) == BRANCH_HUNT)
        assert(planTrigger(0, FIELDS, true, false) == BRANCH_BOOK)
        assert(planTrigger(0, GROUNDS, false, true) == BRANCH_BOOK)
        assert(planTrigger(0, FIELDS, false, false) == BRANCH_DISABLED)
    end)

    it('finish cost and afford', function()
        assert(planFinishCost(100, 50, true) == 50)
        assert(planFinishCost(100, 50, false) == 100)
        assert(planFinishCost(nil, 50, true) == nil)
        assert(planCanAfford(50, 50) and not planCanAfford(51, 50))
        assert(planCanAfford(nil, 0))
    end)

    it('food blocked when full', function()
        assert(planFoodBlocked(true, true))
        assert(not planFoodBlocked(true, false))
        assert(not planFoodBlocked(false, true))
    end)

    it('protect tier by level', function()
        local p, t = planProtectTier(1)
        assert(p == 20 and t == 1)
        p, t = planProtectTier(27)
        assert(p == 50 and t == 2)
        p, t = planProtectTier(76)
        assert(p == 220 and t == 5)
    end)

    it('strips repeat bit from option', function()
        local rep, opt = planRepeatFlag(0x80000007)
        assert(rep and opt == 7)
        rep, opt = planRepeatFlag(5)
        assert(not rep and opt == 5)
    end)
end)
