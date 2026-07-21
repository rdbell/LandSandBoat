-----------------------------------
-- Pure system tests for conquest purchase / outpost / signet pure plans (slice 6197).
-----------------------------------

describe('conquest purchase pure plan', function()
    local OTHER = 4
    local ELSHIMO_UPLANDS = 15

    -- hasOutpost: teleport bit, else UNLOCK_OUTPOST_WARPS 2=all, 1=region<=15
    local function planHasOutpost(hasTeleport, unlockWarps, region)
        if hasTeleport then
            return true
        end
        if unlockWarps == 2 then
            return true
        end
        if unlockWarps == 1 then
            return region <= ELSHIMO_UPLANDS
        end
        return false
    end

    local ALLOW_ALL = 0x3F40001F
    local ALLOW_EXCEPT_TULIA = 0x3FE0001F

    local function planAllowedTeleports(unlockWarps, canTeleport)
        if unlockWarps == 2 then
            return ALLOW_ALL
        end
        if unlockWarps == 1 then
            return ALLOW_EXCEPT_TULIA
        end
        local allowed = ALLOW_ALL
        for region = 0, 18 do
            if not canTeleport[region] then
                allowed = bit.bor(allowed, bit.lshift(1, region + 5))
            end
        end
        return allowed
    end

    -- foreign price: rank~=nil and nation~=guard and guard~=OTHER
    local function planStockPrice(cp, hasRank, pNation, guardNation)
        local price = cp
        if hasRank and pNation ~= guardNation and guardNation ~= OTHER then
            if price <= 8000 then
                price = price * 2
            else
                price = price + 8000
            end
        end
        return price
    end

    -- can purchase: stock, cheat localVar, rank, CP
    local function planCanPurchase(stockItem, boughtItem, stockRank, pRank, price, playerCP)
        if stockItem == nil then
            return -1
        end
        if stockItem ~= boughtItem then
            return -1
        end
        if stockRank and pRank < stockRank then
            return -1
        end
        if playerCP < price then
            return -1
        end
        return price
    end

    local function planSignetDuration(pRank, nationRank)
        return (pRank + nationRank + 3) * 3600
    end

    it('has outpost unlock settings', function()
        assert(planHasOutpost(true, 0, 0))
        assert(planHasOutpost(false, 2, 18))
        assert(planHasOutpost(false, 1, 15))
        assert(not planHasOutpost(false, 1, 16))
        assert(not planHasOutpost(false, 0, 0))
    end)

    it('allowed teleports', function()
        assert(planAllowedTeleports(2, {}) == ALLOW_ALL)
        assert(planAllowedTeleports(1, {}) == ALLOW_EXCEPT_TULIA)
        local can = {}
        for i = 0, 18 do can[i] = true end
        can[0] = false
        local a = planAllowedTeleports(0, can)
        assert(bit.band(a, bit.lshift(1, 5)) ~= 0) -- region 0 blocked bit set
    end)

    it('stock price and purchase', function()
        assert(planStockPrice(1000, true, 0, 1) == 2000)
        assert(planStockPrice(9000, true, 0, 1) == 17000)
        assert(planStockPrice(1000, true, 0, 0) == 1000) -- same nation
        assert(planStockPrice(1000, false, 0, 1) == 1000) -- no rank req
        assert(planStockPrice(1000, true, 0, OTHER) == 1000)
        assert(planCanPurchase(100, 100, nil, 1, 500, 500) == 500)
        assert(planCanPurchase(100, 99, nil, 1, 500, 500) == -1)
        assert(planCanPurchase(100, 100, 5, 4, 500, 500) == -1)
        assert(planCanPurchase(100, 100, nil, 1, 500, 499) == -1)
    end)

    it('signet duration', function()
        assert(planSignetDuration(1, 1) == 5 * 3600)
        assert(planSignetDuration(10, 1) == 14 * 3600)
    end)
end)
