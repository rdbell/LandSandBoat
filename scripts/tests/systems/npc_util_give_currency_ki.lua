-----------------------------------
-- Pure system tests for giveCurrency / giveKeyItem pure plans (slice 6175).
-----------------------------------

describe('npcutil giveCurrency and giveKeyItem pure plan', function()
    local function planGiveCurrency(currency, amount, rate, useTreasurePoolMsg)
        if type(currency) ~= 'string' or type(amount) ~= 'number' then
            return { ok = false, reason = 'params' }
        end
        currency = string.lower(currency)
        local types = { gil = true, bayld = true }
        if not types[currency] then
            return { ok = false, reason = 'currency' }
        end
        local final = math.floor(amount * rate)
        return {
            ok = true,
            currency = currency,
            amount = final,
            isGil = currency == 'gil',
            useTreasurePoolMsg = useTreasurePoolMsg == true,
        }
    end

    -- Normalize KI list: single → { single }; filter already owned.
    local function planGiveKeyItems(keyitems, owned)
        local list = type(keyitems) == 'table' and keyitems or { keyitems }
        local toAdd = {}
        for _, ki in ipairs(list) do
            if not owned[ki] then
                toAdd[#toAdd + 1] = ki
            end
        end
        return { ok = true, toAdd = toAdd }
    end

    it('rejects invalid giveCurrency params', function()
        assert(not planGiveCurrency(1, 10, 1, false).ok)
        assert(not planGiveCurrency('gil', 'x', 1, false).ok)
        assert(not planGiveCurrency('zeny', 10, 1, false).ok)
    end)

    it('applies rate and gil flag', function()
        local r = planGiveCurrency('GIL', 100, 2, false)
        assert(r.ok and r.currency == 'gil' and r.amount == 200 and r.isGil)
        r = planGiveCurrency('bayld', 50, 1.5, true)
        assert(r.ok and r.amount == 75 and not r.isGil and r.useTreasurePoolMsg)
    end)

    it('normalizes and filters key items', function()
        local r = planGiveKeyItems(10, {})
        assert(r.ok and #r.toAdd == 1 and r.toAdd[1] == 10)
        r = planGiveKeyItems({ 10, 20, 30 }, { [20] = true })
        assert(#r.toAdd == 2 and r.toAdd[1] == 10 and r.toAdd[2] == 30)
        r = planGiveKeyItems({ 20 }, { [20] = true })
        assert(#r.toAdd == 0)
    end)
end)
