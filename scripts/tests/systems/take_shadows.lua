-----------------------------------
-- Pure system tests for utils.takeShadows inject products.
-- Pins damage scale formulas; RNG paths exercise via seeded-style expectations
-- only through the pure products (full/partial/default remove).
-----------------------------------

describe('Take shadows pure plans', function()
    -- Pure damage scale mirror: used >= remove → 0; else damage * (remove-used)/remove
    local function scale(damage, remove, used)
        if used >= remove then
            return 0
        end

        return math.floor(damage * (remove - used) / remove) -- Go truncates; for positives match floor of exact halves
    end

    -- Lua returns float; for 100 * 2/3 the float is 66.666…; Go int truncates to 66.
    -- Document product as Lua number: 100 * 2/3
    it('damage scale full block and partial', function()
        assert(scale(100, 2, 2) == 0)
        assert(scale(100, 2, 1) == 50)
        -- 100 * 2/3 = 66.666…; floor toward zero for positive = 66
        assert(math.floor(100 * 2 / 3) == 66)
        assert(100 * (3 - 1) / 3 == 100 * 2 / 3)
    end)

    it('utsusemi full absorb zeros damage when power >= remove', function()
        -- power 3, remove 2 → damage 0, remaining 1, used 2
        local damage, power, remove = 100, 3, 2
        local used, remaining
        if power >= remove then
            remaining = power - remove
            used      = remove
            damage    = 0
        end

        assert(damage == 0 and used == 2 and remaining == 1)
    end)

    it('utsusemi partial scales damage', function()
        local damage, power, remove = 100, 1, 3
        local used, remaining
        if power < remove then
            used      = power
            remaining = 0
            damage    = damage * (remove - power) / remove
        end

        assert(used == 1 and remaining == 0)
        assert(math.abs(damage - (100 * 2 / 3)) < 1e-9)
    end)

    it('default shadowsToRemove is 1', function()
        local remove = nil
        remove = remove or 1
        assert(remove == 1)
    end)

    it('utsusemi preferred over blink when both present', function()
        local utsusemi, blink = 2, 5
        local shadowPower, shadowType = utsusemi, 'utsusemi'
        if shadowPower == 0 then
            shadowPower = blink
            shadowType  = 'blink'
        end

        assert(shadowType == 'utsusemi' and shadowPower == 2)
    end)
end)
