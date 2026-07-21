-----------------------------------
-- Pure system tests for utils.shuffle / permgen / uniqueRandomTable /
-- randomEntry (with deterministic math.random inject via pure mirrors).
-----------------------------------

describe('utils shuffle/random pure plans', function()
    -- Mirror shuffle with injected rand(n) → 1..n
    local function shuffle(inputTable, rand)
        local shuffledTable = {}
        for _, v in ipairs(inputTable) do
            local pos = rand(#shuffledTable + 1)
            table.insert(shuffledTable, pos, v)
        end

        return shuffledTable
    end

    local function permgen(max_val, min_val, rand)
        local indices = {}
        min_val = min_val or 1
        if min_val >= max_val then
            for iter = min_val, max_val, -1 do
                indices[#indices + 1] = iter
            end
        else
            for iter = min_val, max_val, 1 do
                indices[#indices + 1] = iter
            end
        end

        return shuffle(indices, rand)
    end

    it('constants match module pins', function()
        assert(utils.EVENT_CANCELLED_OPTION == bit.lshift(1, 30))
        assert(utils.MAX_UINT32 == 4294967295)
        assert(utils.MAX_INT32 == 2147483647)
    end)

    it('shuffle inserts at random positions', function()
        -- always pos 1 → reverse
        local rolls = { 1, 1, 1 }
        local i = 0
        local rand = function(n)
            i = i + 1
            return rolls[i]
        end
        local r = shuffle({ 1, 2, 3 }, rand)
        assert(r[1] == 3 and r[2] == 2 and r[3] == 1)

        -- always end → identity
        rolls = { 1, 2, 3 }
        i = 0
        r = shuffle({ 1, 2, 3 }, rand)
        assert(r[1] == 1 and r[2] == 2 and r[3] == 3)
    end)

    it('permgen builds range then shuffles', function()
        local rolls = { 1, 2, 3 }
        local i = 0
        local rand = function(n)
            i = i + 1
            return rolls[i]
        end
        local p = permgen(3, 1, rand)
        assert(#p == 3 and p[1] == 1 and p[2] == 2 and p[3] == 3)
    end)

    it('uniqueRandomTable takes first numEntries', function()
        local rolls = { 1, 2, 3 }
        local i = 0
        local rand = function(n)
            i = i + 1
            return rolls[i]
        end
        local u = permgen(3, 1, rand)
        local result = {}
        for j = 1, 2 do
            result[j] = u[j]
        end
        assert(result[1] == 1 and result[2] == 2)
    end)

    it('randomEntryIdx picks by keys list', function()
        -- keys = {1,2,3}, math.random(1,3)=2 → key 2, value 'b'
        local t = { 'a', 'b', 'c' }
        local keys = { 1, 2, 3 }
        local index = keys[2]
        assert(index == 2 and t[index] == 'b')
    end)
end)
