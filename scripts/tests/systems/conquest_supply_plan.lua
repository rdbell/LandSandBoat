-----------------------------------
-- Pure system tests for conquest supply-run pure plans (slice 6196).
-----------------------------------

describe('conquest supply pure plan', function()
    local CITY, FOREIGN, OUTPOST = 1, 2, 3

    -- region > 0 and fresh <= now
    local function planRotten(region, fresh, now)
        return region > 0 and fresh <= now
    end

    -- city/foreign use CONQUEST+40 message path; outpost+ use CONQUEST-1
    local function planRottenCityMsg(guardType)
        return guardType <= FOREIGN
    end

    local function planCanDeliver(questRegion, guardRegion, hasKI)
        return questRegion == guardRegion and hasKI
    end

    -- base mask when not waiting / not holding KI
    local BASE_MASK = 0x7F00001F
    local WAIT_MASK = 0xFFFFFFFF
    local HOLDING_MASK = -1

    local function planSupplyMask(startedToday, holdingKI, owners, hasDarknessNamed)
        -- owners[i] = nation owner for region i, or nil
        -- nation is the player's nation for availability
        local nation = 0 -- sandoria for tests; pass as param via owners match
        return planSupplyMaskFor(startedToday, holdingKI, 0, owners, hasDarknessNamed)
    end

    function planSupplyMaskFor(startedToday, holdingKI, nation, owners, hasDarknessNamed)
        local mask = BASE_MASK
        if startedToday then
            return WAIT_MASK
        end
        if holdingKI then
            return HOLDING_MASK
        end
        for i = 0, 18 do
            if
                owners[i] ~= nation or
                i == 16 or
                i == 17 or
                (i == 18 and not hasDarknessNamed)
            then
                mask = mask + 2 ^ (i + 5)
            end
        end
        return mask
    end

    -- exp ring: allow if multiple rings allowed OR no ring 15761..15763
    -- and (bypass weekly OR recharge var == 0)
    local function planCanBuyExpRing(allowMultiple, hasAnyRing, bypassWeekly, rechargeVar)
        if allowMultiple ~= 1 then
            if hasAnyRing then
                return false
            end
        end
        if bypassWeekly ~= 1 and rechargeVar ~= 0 then
            return false
        end
        return true
    end

    -- regions mask: bit.lshift(1, region+5) for owned regions
    local function planRegionsMask(ownedRegions)
        local mask = 0
        for _, region in ipairs(ownedRegions) do
            mask = bit.bor(mask, bit.lshift(1, region + 5))
        end
        return mask
    end

    it('rotten and deliver', function()
        assert(planRotten(1, 100, 100))
        assert(planRotten(1, 100, 200))
        assert(not planRotten(0, 100, 200))
        assert(not planRotten(1, 200, 100))
        assert(planRottenCityMsg(CITY) and planRottenCityMsg(FOREIGN))
        assert(not planRottenCityMsg(OUTPOST))
        assert(planCanDeliver(5, 5, true))
        assert(not planCanDeliver(5, 6, true))
        assert(not planCanDeliver(5, 5, false))
    end)

    it('supply available mask', function()
        assert(planSupplyMaskFor(true, false, 0, {}, true) == WAIT_MASK)
        assert(planSupplyMaskFor(false, true, 0, {}, true) == HOLDING_MASK)
        -- all regions owned by nation 0, has darkness named
        local owners = {}
        for i = 0, 18 do owners[i] = 0 end
        local m = planSupplyMaskFor(false, false, 0, owners, true)
        -- still block 16 and 17 always
        assert(m == BASE_MASK + 2^(16+5) + 2^(17+5))
        -- without darkness named, also block 18
        m = planSupplyMaskFor(false, false, 0, owners, false)
        assert(m == BASE_MASK + 2^(16+5) + 2^(17+5) + 2^(18+5))
    end)

    it('exp ring buy gates', function()
        assert(planCanBuyExpRing(1, true, 1, 1)) -- allow all
        assert(not planCanBuyExpRing(0, true, 1, 0)) -- has ring
        assert(planCanBuyExpRing(0, false, 1, 5)) -- bypass weekly
        assert(not planCanBuyExpRing(0, false, 0, 1)) -- weekly lock
        assert(planCanBuyExpRing(0, false, 0, 0))
    end)

    it('regions ownership mask', function()
        assert(planRegionsMask({0}) == bit.lshift(1, 5))
        assert(planRegionsMask({0, 1}) == bit.bor(bit.lshift(1, 5), bit.lshift(1, 6)))
    end)
end)
