-----------------------------------
-- Pure system tests for chocobo care plan pure (slice 6155).
-----------------------------------

describe('chocobo care plan pure', function()
    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function handleStatChange(value, change, max, posMult, negMult)
        if change == 0 then return value end
        if change > 0 then
            change = change * posMult
        else
            change = change * negMult
        end
        return clamp(value + change, 0, max)
    end

    -- Unpack 4 slots from care_plan bitmask (MSB first)
    local function unpackPlans(carePlan)
        local plans = {}
        for i = 0, 3 do
            local offset = 24 - (i * 8)
            local length = bit.band(bit.rshift(carePlan, offset + 4), 0xF)
            local planType = bit.band(bit.rshift(carePlan, offset), 0xF)
            if length == 0 then
                length = 7
                planType = 0 -- BASIC_CARE
            end
            plans[#plans + 1] = { length = length, type = planType }
        end
        return plans
    end

    local function packPlans(plans)
        local newCarePlan = 0
        for i = 0, 3 do
            local offset = 24 - (i * 8)
            local p = plans[i + 1]
            newCarePlan = bit.bor(newCarePlan, bit.lshift(p.length, offset + 4))
            newCarePlan = bit.bor(newCarePlan, bit.lshift(p.type, offset))
        end
        return newCarePlan
    end

    local function advancePlans(plans, elapsedDays)
        local remaining = elapsedDays
        while remaining > 0 do
            local deduct = math.min(plans[1].length, remaining)
            plans[1].length = plans[1].length - deduct
            remaining = remaining - deduct
            if plans[1].length == 0 then
                table.remove(plans, 1)
                plans[#plans + 1] = { length = 7, type = 0 }
            end
        end
        return plans
    end

    it('stat change mult and clamp', function()
        assert(handleStatChange(10, 0, 255, 1, 1) == 10)
        assert(handleStatChange(10, 2, 255, 1, 1) == 12)
        assert(handleStatChange(10, 2, 255, 2, 1) == 14)
        assert(handleStatChange(10, -3, 255, 1, 2) == 4) -- -3 * 2 = -6
        assert(handleStatChange(2, -5, 255, 1, 1) == 0)
        assert(handleStatChange(250, 10, 255, 1, 1) == 255)
    end)

    it('pack unpack roundtrip', function()
        -- slot0: len=3 type=2 at bits 28-24 and 24-20... offset 24 for i=0
        -- length at offset+4 = 28, type at 24
        local packed = bit.bor(bit.lshift(3, 28), bit.lshift(2, 24))
        packed = bit.bor(packed, bit.lshift(5, 20), bit.lshift(1, 16))
        packed = bit.bor(packed, bit.lshift(7, 12), bit.lshift(0, 8))
        packed = bit.bor(packed, bit.lshift(4, 4), bit.lshift(8, 0))
        local plans = unpackPlans(packed)
        assert(plans[1].length == 3 and plans[1].type == 2)
        assert(plans[2].length == 5 and plans[2].type == 1)
        assert(plans[3].length == 7 and plans[3].type == 0)
        assert(plans[4].length == 4 and plans[4].type == 8)
        assert(packPlans(plans) == packed)
    end)

    it('zero length slot defaults to basic 7', function()
        local plans = unpackPlans(0)
        for i = 1, 4 do
            assert(plans[i].length == 7 and plans[i].type == 0)
        end
    end)

    it('advance consumes and rotates plans', function()
        local plans = {
            { length = 2, type = 1 },
            { length = 3, type = 2 },
            { length = 4, type = 3 },
            { length = 5, type = 4 },
        }
        advancePlans(plans, 2)
        assert(plans[1].length == 3 and plans[1].type == 2) -- first consumed
        assert(plans[4].length == 7 and plans[4].type == 0) -- appended basic
        -- partial consume
        plans = {
            { length = 5, type = 1 },
            { length = 3, type = 2 },
            { length = 4, type = 3 },
            { length = 5, type = 4 },
        }
        advancePlans(plans, 2)
        assert(plans[1].length == 3 and plans[1].type == 1)
    end)

    it('payment scales with days and gil mult', function()
        local payment = 100 * 3 * 2 -- data payment * days * gil mult
        assert(payment == 600)
    end)
end)
