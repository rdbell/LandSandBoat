-----------------------------------
-- Pure system tests for conquest fee/arg pure plans (slice 6195).
-----------------------------------

describe('conquest event pure plan', function()
    local SANDORIA, BASTOK, WINDURST, OTHER = 0, 1, 2, 4
    local CITY, FOREIGN, OUTPOST, BORDER = 1, 2, 3, 4

    local function areAllies(isAlliance, rankA, rankB)
        return isAlliance and rankA > 1 and rankB > 1
    end

    local function setHomepointFee(pNation, guardNation, allies, rank)
        if pNation ~= guardNation and not allies then
            if rank <= 5 then
                return 100 * (2 ^ (rank - 1))
            end
            return (800 * rank) - 2400
        end
        return 0
    end

    local function getArg6(rank, nation)
        return rank + (nation * 32)
    end

    local function getArg1(guardNation, pNation, guardType, allies, hasVoucher, campaignActive)
        local output = 0
        local signet = 0
        local cipher = campaignActive * 20 * 65536
        local voucher = hasVoucher and 0x20000 or 0

        if guardNation == WINDURST then
            output = 33
        elseif guardNation == SANDORIA then
            output = 1
        elseif guardNation == BASTOK then
            output = 17
        end

        if guardNation == pNation then
            signet = 0
        else
            signet = 7
            if allies then
                signet = 2 ^ (2 - pNation)
            end
        end

        if guardNation == OTHER then
            output = (pNation * 16) + (3 * 256) + 65537
        else
            output = output + 256 * signet
        end

        if guardType == CITY then
            output = output + voucher
        end

        if guardType >= OUTPOST then
            output = output - 1
        end

        if output >= 1792 and guardType >= OUTPOST then
            output = 1808
        end

        return output + cipher
    end

    it('areAllies gate', function()
        assert(areAllies(true, 2, 3))
        assert(not areAllies(false, 2, 3))
        assert(not areAllies(true, 1, 3))
        assert(not areAllies(true, 2, 1))
    end)

    it('homepoint fee', function()
        assert(setHomepointFee(SANDORIA, SANDORIA, false, 5) == 0)
        assert(setHomepointFee(SANDORIA, BASTOK, true, 5) == 0)
        assert(setHomepointFee(SANDORIA, BASTOK, false, 1) == 100)
        assert(setHomepointFee(SANDORIA, BASTOK, false, 3) == 400)
        assert(setHomepointFee(SANDORIA, BASTOK, false, 5) == 1600)
        assert(setHomepointFee(SANDORIA, BASTOK, false, 6) == 2400)
        assert(setHomepointFee(SANDORIA, BASTOK, false, 10) == 5600)
    end)

    it('arg6 rank nationality', function()
        assert(getArg6(1, SANDORIA) == 1)
        assert(getArg6(5, BASTOK) == 5 + 32)
        assert(getArg6(10, WINDURST) == 10 + 64)
    end)

    it('arg1 overseer packing', function()
        -- same nation city, no voucher, no campaign
        local a = getArg1(SANDORIA, SANDORIA, CITY, false, false, 0)
        assert(a == 1) -- base 1, signet 0
        -- foreign non-ally: signet 7
        a = getArg1(SANDORIA, BASTOK, CITY, false, false, 0)
        assert(a == 1 + 256 * 7)
        -- foreign ally pNation=BASTOK: signet 2^(2-1)=2
        a = getArg1(SANDORIA, BASTOK, CITY, true, false, 0)
        assert(a == 1 + 256 * 2)
        -- voucher on city
        a = getArg1(SANDORIA, SANDORIA, CITY, false, true, 0)
        assert(a == 1 + 0x20000)
        -- outpost subtract 1
        a = getArg1(SANDORIA, SANDORIA, OUTPOST, false, false, 0)
        assert(a == 0)
        -- OTHER nation pack
        a = getArg1(OTHER, SANDORIA, CITY, false, false, 0)
        assert(a == (0 * 16) + (3 * 256) + 65537)
        -- cipher when campaign active=1
        a = getArg1(SANDORIA, SANDORIA, CITY, false, false, 1)
        assert(a == 1 + 20 * 65536)
        -- clamp to 1808 for high output outpost
        a = getArg1(WINDURST, BASTOK, OUTPOST, false, false, 0)
        -- base 33 + 256*7 - 1 = 33+1792-1=1824 >= 1792 → 1808
        assert(a == 1808)
    end)
end)
