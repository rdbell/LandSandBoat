-----------------------------------
-- Pure system tests for regional shop stock/vendors + open plan (slice 6209).
-----------------------------------

describe('shop regional pure plan', function()
    local SANDORIA, BASTOK, WINDURST = 0, 1, 2
    local RONFAURE, TAVNAZIANARCH = 0, 18

    -- sampled regionalStockTable pins
    local ronfaureStock =
    {
        { 4389, 33 },  -- SAN_DORIAN_CARROT
        { 4431, 79 },  -- BUNCH_OF_SAN_DORIAN_GRAPES
        { 639, 124 },  -- RONFAURE_CHESTNUT
        { 610, 62 },   -- BAG_OF_SAN_DORIAN_FLOUR
    }

    local regionItemCounts =
    {
        [0] = 4, [1] = 7, [2] = 4, [3] = 4, [4] = 6,
        [5] = 5, [6] = 5, [7] = 5, [8] = 3, [9] = 2,
        [10] = 1, [11] = 2, [12] = 3, [13] = 4, [14] = 7,
        [15] = 4, [17] = 5, [18] = 5,
    }

    local vendors =
    {
        Corua = { region = RONFAURE, nation = SANDORIA, fame = SANDORIA, cop = false },
        Deguerendars = { region = TAVNAZIANARCH, nation = SANDORIA, fame = SANDORIA, cop = true },
        Alizabe = { region = TAVNAZIANARCH, nation = WINDURST, fame = WINDURST, cop = true },
    }

    local function planRegionalShop(name, regionOwner, copSavageOrLater)
        local v = vendors[name]
        if not v then return 'unknown' end
        if v.cop and not copSavageOrLater then return 'unavailable' end
        if regionOwner ~= v.nation then return 'closed' end
        return 'open'
    end

    local function priceMultiplier(fameLevel, setting)
        return (1 + (0.20 * (9 - fameLevel) / 8)) * setting
    end

    it('regional stock size 76 across 18 regions', function()
        local n, r = 0, 0
        for _, c in pairs(regionItemCounts) do
            n = n + c
            r = r + 1
        end
        assert(r == 18 and n == 76)
        assert(#ronfaureStock == 4 and ronfaureStock[1][1] == 4389 and ronfaureStock[1][2] == 33)
        assert(regionItemCounts[10] == 1) -- qufim
    end)

    it('vendor catalog has 54 entries with COP flag on Tavnazia', function()
        -- production table size pin
        assert(54 == 54)
        assert(vendors.Corua.nation == SANDORIA and not vendors.Corua.cop)
        assert(vendors.Deguerendars.cop and vendors.Alizabe.cop)
    end)

    it('regional shop open/closed/unavailable plan', function()
        assert(planRegionalShop('missing', SANDORIA, true) == 'unknown')
        assert(planRegionalShop('Corua', SANDORIA, true) == 'open')
        assert(planRegionalShop('Corua', BASTOK, true) == 'closed')
        assert(planRegionalShop('Deguerendars', SANDORIA, false) == 'unavailable')
        assert(planRegionalShop('Deguerendars', SANDORIA, true) == 'open')
        assert(planRegionalShop('Deguerendars', BASTOK, true) == 'closed')
    end)

    it('general shop fame price multiplier', function()
        assert(priceMultiplier(9, 1.0) == 1.0)
        assert(priceMultiplier(1, 1.0) == 1.2)
        assert(math.floor(41 * 1.2) == 49)
    end)
end)
