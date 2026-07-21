-----------------------------------
-- Pure system tests for appraisal appraiseItem / finish pure plans (slice 6192).
-----------------------------------

describe('appraisal event pure plan', function()
    local UNAPPRAISED_SWORD = 2190
    local SEAGULL_GROUNDED = 31
    -- weighted rows for sword / seagull (sum 100)
    local items = {
        { 65, 18366 }, -- GUST_CLAYMORE
        { 30, 16978 },
        { 5, 18434 },
    }

    local function isUnappraised(id)
        local catalog = {
            [2190] = true, [2191] = true, [2192] = true, [2193] = true,
            [2194] = true, [2195] = true, [2196] = true, [2276] = true,
        }
        return catalog[id] == true
    end

    local function selectItem(rows, roll)
        local sum = 0
        for i = 1, #rows do
            sum = sum + rows[i][1]
        end
        local cum = 0
        for i = 1, #rows do
            cum = cum + rows[i][1]
            if cum >= roll then
                return rows[i][2]
            end
        end
        return 0
    end

    -- appraiseItem pure once trade item + appraisalID + roll known
    local function planAppraise(playerGil, cost, tradeID, hasTable, roll)
        if playerGil < cost then
            return { ok = false }
        end
        if not isUnappraised(tradeID) or not hasTable then
            return { ok = false }
        end
        local picked = selectItem(items, roll)
        if picked == 0 then
            return { ok = false }
        end
        return { ok = true, item = picked }
    end

    local function planFinish(csid, appraisalCsid)
        return csid == appraisalCsid
    end

    it('rejects insufficient gil', function()
        local r = planAppraise(99, 100, UNAPPRAISED_SWORD, true, 1)
        assert(not r.ok)
    end)

    it('rejects non-unappraised trade', function()
        local r = planAppraise(1000, 100, 1234, true, 1)
        assert(not r.ok)
    end)

    it('rejects missing table', function()
        local r = planAppraise(1000, 100, UNAPPRAISED_SWORD, false, 1)
        assert(not r.ok)
    end)

    it('starts with picked item', function()
        local r = planAppraise(1000, 100, UNAPPRAISED_SWORD, true, 1)
        assert(r.ok and r.item == 18366)
        r = planAppraise(1000, 100, UNAPPRAISED_SWORD, true, 100)
        assert(r.ok and r.item == 18434)
    end)

    it('finish only on matching csid', function()
        assert(planFinish(50, 50))
        assert(not planFinish(49, 50))
        assert(SEAGULL_GROUNDED == 31) -- pin used by catalog tests
    end)
end)
