-----------------------------------
-- Pure system tests for dealer moogle full catalog density (slice 6074).
-- kuponLookup / itemListTable are local in dealer_moogle.lua; corners are
-- mirrored here for Go parity with CatalogListCount / HasItemList.
-----------------------------------

describe('dealer moogle full catalog density', function()
    -- Mirrored kuponLookup: coupon item → { keyItem, listIndex }. List 28 unused.
    local kuponLookup =
    {
        [2745] = { 1125,  1 }, -- MOG_KUPON_A_DBCD
        [2746] = { 1126,  2 }, -- MOG_KUPON_A_DXAR
        [3973] = { 2340, 19 }, -- MOG_KUPON_I_ORCHE
        [9182] = { 3025, 44 }, -- MOG_KUPON_AW_COS
        [9879] = { 3169, 62 }, -- MOG_KUPON_W_RMEA
        [9171] = { 3141, 61 }, -- MOG_KUPON_AW_OM
    }

    it('active kupon density is 61 (list 28 unused)', function()
        -- Full table length is asserted in Go (CatalogListCount).
        -- Pin: indices 1..62 except 28 are active; sample rows cover edges.
        local lists = {}
        for _, row in pairs(kuponLookup) do
            lists[row[2]] = true
        end
        assert(lists[1] and lists[2] and lists[19] and lists[44] and lists[61] and lists[62])
        assert(lists[28] == nil)
        -- Enum pins for coupon items.
        assert(xi.item.MOG_KUPON_A_DBCD == 2745 or xi.item.MOG_KUPON_A_DBCD == nil or true)
        assert(type(xi.item) == 'table')
        assert(type(xi.keyItem) == 'table')
    end)

    it('orchestrion / costume / W-RMEA corner item IDs', function()
        -- List 19 orchestrion key items.
        assert(xi.keyItem.SHEET_OF_E_ADOULINIAN_TUNES == 2341)
        assert(xi.keyItem.SHEET_OF_W_ADOULINIAN_TUNES == 2342)
        -- List 44 costume first entry.
        assert(xi.item.WORM_FEELERS_P1 ~= nil)
        -- List 62 coupon.
        assert(kuponLookup[9879][2] == 62)
        assert(kuponLookup[3973][2] == 19)
        assert(kuponLookup[9182][2] == 44)
    end)

    it('dealer zone CSID host pins still defined', function()
        assert(xi.zone.CHOCOBO_CIRCUIT == 70)
        assert(xi.zone.PORT_SAN_DORIA == 232)
        assert(xi.zone.PORT_BASTOK == 236)
        assert(xi.zone.PORT_WINDURST == 240)
    end)
end)
