-----------------------------------
-- Pure system tests for regime bookOnEventUpdate pure plan (slice 6183).
-----------------------------------

describe('regime bookOnEventUpdate pure plan', function()
    -- killed display: only show killed if needed ~= 0
    local function displayKilled(needed, killed)
        if needed ~= 0 then
            return killed
        end
        return 0
    end

    -- update branch from opt flags
    local function planUpdate(opt, page, killed, selectPage)
        if not opt then
            return { ok = false }
        end
        if opt.review and page then
            return {
                kind = 'review',
                args = {
                    page[1], page[2], page[3], page[4],
                    displayKilled(page[1], killed[1]),
                    displayKilled(page[2], killed[2]),
                    displayKilled(page[3], killed[3]),
                    displayKilled(page[4], killed[4]),
                },
            }
        elseif opt.details and page then
            return {
                kind = 'details',
                args = { 0, 0, 0, 0, 0, page[5], page[6], 0 },
            }
        elseif opt.prowess then
            return {
                kind = 'prowess',
                args = { 0, 0, 0, 0, 0, 0, 0, 0 },
            }
        elseif opt.page and selectPage then
            return {
                kind = 'page',
                args = {
                    selectPage[1], selectPage[2], selectPage[3], selectPage[4],
                    0, selectPage[5], selectPage[6], selectPage[8],
                },
            }
        end
        return { ok = false }
    end

    it('reviews progress with zero-needed slots blanked', function()
        local page = { 2, 0, 3, 0, 10, 20, 100, 55 }
        local killed = { 1, 9, 2, 7 }
        local r = planUpdate({ review = true }, page, killed, nil)
        assert(r.kind == 'review')
        assert(r.args[1] == 2 and r.args[2] == 0 and r.args[3] == 3 and r.args[4] == 0)
        assert(r.args[5] == 1 and r.args[6] == 0) -- slot2 needed 0 → killed display 0
        assert(r.args[7] == 2 and r.args[8] == 0)
    end)

    it('details shows level range', function()
        local page = { 1, 1, 0, 0, 15, 25, 50, 99 }
        local r = planUpdate({ details = true }, page, { 0, 0, 0, 0 }, nil)
        assert(r.kind == 'details')
        assert(r.args[6] == 15 and r.args[7] == 25 and r.args[8] == 0)
    end)

    it('prowess returns zeros', function()
        local r = planUpdate({ prowess = true }, nil, nil, nil)
        assert(r.kind == 'prowess')
        for i = 1, 8 do
            assert(r.args[i] == 0)
        end
    end)

    it('page select packs needs levels and regime id', function()
        local sel = { 4, 5, 6, 0, 30, 40, 200, 77 }
        local r = planUpdate({ page = 2 }, nil, nil, sel)
        assert(r.kind == 'page')
        assert(r.args[1] == 4 and r.args[4] == 0 and r.args[5] == 0)
        assert(r.args[6] == 30 and r.args[7] == 40 and r.args[8] == 77)
    end)

    it('invalid option fails', function()
        assert(not planUpdate(nil, { 1 }, { 0 }, nil).ok)
    end)
end)
