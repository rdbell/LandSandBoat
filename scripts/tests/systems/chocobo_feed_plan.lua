-----------------------------------
-- Pure system tests for chocobo FEED_CHOCOBO pure plan (slice 6162).
-----------------------------------

describe('chocobo feed pure plan', function()
    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    -- food: { hunger, affection, energy, ... glow }
    local foods = {
        [1] = { hunger = 25, energy = 0, glow = 96 },
        [2] = { hunger = 50, energy = 100, glow = 98 },
    }

    local heal = {
        [0] = { 1 }, -- ILL cured by item 1
        [3] = { 1 }, -- INJURED cured by item 1
    }

    local function planFeed(p)
        local food = foods[p.itemId]
        if not food then
            return { ok = false }
        end
        local hunger = clamp(p.hunger + food.hunger, 0, 255)
        local energy = clamp(p.energy + food.energy, 0, 100)
        local conditions = p.conditions
        local cleared = {}
        for cond, items in pairs(heal) do
            if bit.band(conditions, bit.lshift(1, cond)) ~= 0 then
                for _, id in ipairs(items) do
                    if id == p.itemId then
                        conditions = bit.band(conditions, bit.bnot(bit.lshift(1, cond)))
                        cleared[#cleared + 1] = cond
                    end
                end
            end
        end
        local glow = food.glow
        if p.foodCount > 1 then
            glow = 99 -- GREEN multi-feed
        end
        return {
            ok = true,
            hunger = hunger,
            energy = energy,
            conditions = conditions,
            glow = glow,
            reaction = 0,
            cleared = cleared,
        }
    end

    it('applies hunger energy clamps', function()
        local r = planFeed({ itemId = 1, hunger = 240, energy = 90, conditions = 0, foodCount = 1 })
        assert(r.ok and r.hunger == 255 and r.energy == 90 and r.glow == 96)
        r = planFeed({ itemId = 2, hunger = 10, energy = 50, conditions = 0, foodCount = 1 })
        assert(r.hunger == 60 and r.energy == 100)
    end)

    it('multi feed forces green glow', function()
        local r = planFeed({ itemId = 1, hunger = 0, energy = 0, conditions = 0, foodCount = 2 })
        assert(r.glow == 99)
    end)

    it('clears healable conditions', function()
        local ill = bit.lshift(1, 0)
        local r = planFeed({ itemId = 1, hunger = 0, energy = 0, conditions = ill, foodCount = 1 })
        assert(bit.band(r.conditions, ill) == 0)
        assert(#r.cleared == 1 and r.cleared[1] == 0)
        -- wrong item does not clear
        r = planFeed({ itemId = 2, hunger = 0, energy = 0, conditions = ill, foodCount = 1 })
        assert(bit.band(r.conditions, ill) ~= 0)
    end)

    it('unknown food fails', function()
        assert(planFeed({ itemId = 99, hunger = 0, energy = 0, conditions = 0, foodCount = 1 }).ok == false)
    end)
end)
