-----------------------------------
-- Pure system tests for utils.getLowestFreeSlot scan logic.
-----------------------------------

describe('Lowest free slot pure plans', function()
    -- Mirror pure scan with pre-sorted slot numbers (1-based).
    local function lowestFree(occupied)
        table.sort(occupied)
        local lowestFreeSlot = 1
        for _, slot in ipairs(occupied) do
            if slot == lowestFreeSlot then
                lowestFreeSlot = lowestFreeSlot + 1
            elseif slot > lowestFreeSlot then
                break
            end
        end

        return lowestFreeSlot
    end

    it('empty occupied returns 1', function()
        assert(lowestFree({}) == 1)
    end)

    it('contiguous 1..n returns n+1', function()
        assert(lowestFree({ 1, 2, 3 }) == 4)
        assert(lowestFree({ 1 }) == 2)
    end)

    it('finds first gap', function()
        assert(lowestFree({ 1, 3 }) == 2)
        assert(lowestFree({ 2, 3 }) == 1)
        assert(lowestFree({ 1, 2, 5 }) == 3)
        assert(lowestFree({ 5 }) == 1)
    end)

    it('unsorted input still finds lowest free after sort', function()
        assert(lowestFree({ 3, 1, 2 }) == 4)
        assert(lowestFree({ 5, 1, 2 }) == 3)
    end)
end)
