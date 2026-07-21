-----------------------------------
-- Pure system tests for appraisalItems full catalog corners (slice 6066).
-----------------------------------

describe('appraisalItems pure catalog', function()
    it('UNAPPRAISED_SWORD RED_VERSUS_BLUE is Kilij only', function()
        local info = xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_SWORD]
        local items = info[xi.assault.mission.RED_VERSUS_BLUE].items
        assert(#items == 1)
        assert(items[1][1] == 100)
        assert(items[1][2] == xi.item.KILIJ)
        assert(xi.item.UNAPPRAISED_SWORD == 2190)
    end)

    it('UNAPPRAISED_DAGGER NYZUL_TOM_TIT_TAT weights', function()
        local info = xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_DAGGER]
        local items = info[xi.appraisal.origin.NYZUL_TOM_TIT_TAT].items
        assert(#items == 3)
        assert(items[1][1] == 70)
        assert(items[1][2] == xi.item.BRONZE_KNIFE)
        assert(items[3][1] == 5)
        assert(items[3][2] == xi.item.FRUIT_PUNCHES)
        assert(xi.appraisal.origin.NYZUL_TOM_TIT_TAT == 108)
    end)

    it('empty SASH INGOT POTION CLOTH tables', function()
        assert(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_SASH] ~= nil)
        assert(next(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_SASH]) == nil)
        assert(next(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_INGOT]) == nil)
        assert(next(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_POTION]) == nil)
        assert(next(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_CLOTH]) == nil)
    end)

    it('BOX and SWORD density corners', function()
        local swordKeys = 0
        for _ in pairs(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_SWORD]) do
            swordKeys = swordKeys + 1
        end
        assert(swordKeys == 15)

        local boxKeys = 0
        for _ in pairs(xi.appraisal.appraisalItems[xi.item.UNAPPRAISED_BOX]) do
            boxKeys = boxKeys + 1
        end
        assert(boxKeys == 21)
        assert(xi.item.UNAPPRAISED_BOX == 2286)
    end)
end)
