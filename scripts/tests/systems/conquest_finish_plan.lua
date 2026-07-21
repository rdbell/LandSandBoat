-----------------------------------
-- Pure system tests for conquest overseer finish branch pure plans (slice 6198).
-----------------------------------

describe('conquest overseer finish pure plan', function()
    local CITY, FOREIGN, OUTPOST, BORDER = 1, 2, 3, 4

    local KIND_NONE = 0
    local KIND_SIGNET = 1
    local KIND_BEGIN_SUPPLY = 2
    local KIND_FINISH_SUPPLY = 3
    local KIND_HOMEPOINT = 4
    local KIND_PURCHASE = 5

    local function planFinish(option, guardType, pNation, guardNation, sRegion, guardRegion, hasSupplyKI, hasOutpostRow)
        if option == 1 then
            return KIND_SIGNET
        end
        if option >= 65541 and option <= 65565 and guardType <= FOREIGN then
            return KIND_BEGIN_SUPPLY
        end
        if
            option == 2 and
            guardType >= OUTPOST and
            sRegion == guardRegion and
            hasOutpostRow and
            hasSupplyKI and
            guardNation == pNation
        then
            return KIND_FINISH_SUPPLY
        end
        if option == 4 then
            return KIND_HOMEPOINT
        end
        if option >= 32768 and option <= 32944 then
            return KIND_PURCHASE
        end
        return KIND_NONE
    end

    local function planSupplyRegion(option)
        return option - 65541
    end

    local function planIsExpRingOption(option)
        return option >= 32933 and option <= 32935
    end

    local function planVendorNationFlag(owner, pNation, allies)
        if owner == pNation then
            return 1
        end
        if allies then
            return 2
        end
        return 0
    end

    it('signet and begin supply', function()
        assert(planFinish(1, CITY, 0, 0, 0, 0, false, false) == KIND_SIGNET)
        assert(planFinish(65541, CITY, 0, 0, 0, 0, false, false) == KIND_BEGIN_SUPPLY)
        assert(planFinish(65565, FOREIGN, 0, 0, 0, 0, false, false) == KIND_BEGIN_SUPPLY)
        assert(planFinish(65541, OUTPOST, 0, 0, 0, 0, false, false) == KIND_NONE) -- outpost can't begin
        assert(planSupplyRegion(65541) == 0)
        assert(planSupplyRegion(65546) == 5)
    end)

    it('finish supply and homepoint', function()
        assert(planFinish(2, OUTPOST, 0, 0, 3, 3, true, true) == KIND_FINISH_SUPPLY)
        assert(planFinish(2, BORDER, 0, 0, 3, 3, true, true) == KIND_FINISH_SUPPLY)
        assert(planFinish(2, OUTPOST, 0, 1, 3, 3, true, true) == KIND_NONE) -- wrong nation
        assert(planFinish(2, OUTPOST, 0, 0, 3, 4, true, true) == KIND_NONE) -- wrong region
        assert(planFinish(2, OUTPOST, 0, 0, 3, 3, false, true) == KIND_NONE) -- no KI
        assert(planFinish(4, CITY, 0, 0, 0, 0, false, false) == KIND_HOMEPOINT)
    end)

    it('purchase and vendor nation', function()
        assert(planFinish(32768, CITY, 0, 0, 0, 0, false, false) == KIND_PURCHASE)
        assert(planFinish(32944, CITY, 0, 0, 0, 0, false, false) == KIND_PURCHASE)
        assert(planFinish(32945, CITY, 0, 0, 0, 0, false, false) == KIND_NONE)
        assert(planIsExpRingOption(32933) and planIsExpRingOption(32935))
        assert(not planIsExpRingOption(32932))
        assert(planVendorNationFlag(0, 0, false) == 1)
        assert(planVendorNationFlag(1, 0, true) == 2)
        assert(planVendorNationFlag(1, 0, false) == 0)
    end)
end)
