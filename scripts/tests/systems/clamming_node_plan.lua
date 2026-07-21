-----------------------------------
-- Pure system tests for clamming nodeOnEventUpdate plan (slice 6147).
-----------------------------------

describe('clamming node pure plan', function()
    local function rateColumn(improved)
        return improved and 1 or 0
    end

    local function rateSum(entries, col)
        local sum = 0
        for _, e in ipairs(entries) do
            sum = sum + e[2 + col]
        end
        return sum
    end

    -- LSB picks first entry where rate <= roll (not cumulative weight).
    local function pickItem(entries, col, roll)
        for _, e in ipairs(entries) do
            if e[2 + col] <= roll then
                return e[1]
            end
        end
        return 0
    end

    local function incidentChance(reduced)
        return reduced and 5 or 10
    end

    local function incidentHits(kitSize, roll, chance)
        return kitSize == 200 and roll <= chance
    end

    local function overload(kitWeight, itemWeight, kitSize)
        return kitWeight + itemWeight > kitSize
    end

    -- entries: { { itemId, rateNormal, rateImproved }, ... }
    local function plan(p)
        -- Early returns
        if not p.csidOK or not p.hasKit then
            return { early = true }
        end
        if p.kitBroken then
            return { early = true, kitBrokenEarly = true }
        end
        if incidentHits(p.kitSize, p.incidentRoll, incidentChance(p.reducedIncidents)) then
            return {
                incident   = true,
                newWeight  = p.kitWeight + 10000,
                kitBroken  = true,
            }
        end
        local col = rateColumn(p.improvedResults)
        local itemId = pickItem(p.entries, col, p.lootRoll)
        local itemWeight = p.itemWeights[itemId] or 0
        local newWeight = p.kitWeight + itemWeight
        if overload(p.kitWeight, itemWeight, p.kitSize) then
            return {
                itemId     = itemId,
                itemWeight = itemWeight,
                newWeight  = newWeight,
                overload   = true,
                kitBroken  = true,
            }
        end
        return {
            itemId     = itemId,
            itemWeight = itemWeight,
            newWeight  = newWeight,
            addItem    = true,
        }
    end

    local sample = {
        { 10, 5000, 4000 },
        { 20, 2000, 1500 },
        { 30,  500,  750 },
        { 40,   10,   15 },
    }
    local weights = { [10] = 5, [20] = 10, [30] = 20, [40] = 50 }

    it('rate column and sum', function()
        assert(rateColumn(false) == 0 and rateColumn(true) == 1)
        assert(rateSum(sample, 0) == 7510)
        assert(rateSum(sample, 1) == 6265)
    end)

    it('picks first entry with rate <= roll', function()
        -- high roll hits first common entry
        assert(pickItem(sample, 0, 5000) == 10)
        assert(pickItem(sample, 0, 9999) == 10)
        -- mid roll skips slug (5000), hits pebble (2000)
        assert(pickItem(sample, 0, 2500) == 20)
        -- low roll reaches rare
        assert(pickItem(sample, 0, 10) == 40)
        -- roll below all rates → 0
        assert(pickItem(sample, 0, 5) == 0)
    end)

    it('incident gates kit size 200', function()
        assert(incidentChance(false) == 10 and incidentChance(true) == 5)
        assert(incidentHits(200, 10, 10) == true)
        assert(incidentHits(200, 11, 10) == false)
        assert(incidentHits(50, 1, 10) == false)
    end)

    it('early returns before loot', function()
        assert(plan({ csidOK = false, hasKit = true }).early == true)
        assert(plan({ csidOK = true, hasKit = false }).early == true)
        local r = plan({ csidOK = true, hasKit = true, kitBroken = true })
        assert(r.early and r.kitBrokenEarly)
    end)

    it('incident path breaks kit', function()
        local r = plan({
            csidOK = true, hasKit = true, kitBroken = false,
            kitSize = 200, kitWeight = 100,
            incidentRoll = 5, reducedIncidents = false,
            improvedResults = false, lootRoll = 1,
            entries = sample, itemWeights = weights,
        })
        assert(r.incident and r.kitBroken and r.newWeight == 10100)
    end)

    it('overload vs add item', function()
        local base = {
            csidOK = true, hasKit = true, kitBroken = false,
            kitSize = 50, kitWeight = 40,
            incidentRoll = 100, reducedIncidents = false,
            improvedResults = false, lootRoll = 5000,
            entries = sample, itemWeights = weights,
        }
        -- item 10 weight 5 → 40+5=45 not overload
        local r = plan(base)
        assert(r.addItem and r.itemId == 10 and r.newWeight == 45 and not r.overload)
        -- item 10 weight 5 → 48+5=53 overload
        base.kitWeight = 48
        r = plan(base)
        assert(r.overload and r.kitBroken and r.itemId == 10 and r.newWeight == 53)
    end)
end)
