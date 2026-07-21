-----------------------------------
-- Pure system tests for chocobo care plan option update (slice 6156).
-----------------------------------

describe('chocobo care plan option pure plan', function()
    local function decode(option)
        return {
            slot   = bit.band(0xF, bit.rshift(option, 8)),
            length = bit.band(0x7, bit.rshift(option, 16)),
            type   = bit.band(0xF, bit.rshift(option, 19)),
        }
    end

    local function defaultCarePlan()
        local defaultSlot = bit.lshift(7, 4) + 0 -- length 7, type BASIC_CARE
        return bit.lshift(defaultSlot, 24) +
            bit.lshift(defaultSlot, 16) +
            bit.lshift(defaultSlot, 8) +
            bit.lshift(defaultSlot, 0)
    end

    local function applySlot(carePlan, slot, length, planType)
        if carePlan == 0 then
            carePlan = defaultCarePlan()
        end
        local careByte = bit.lshift(length, 4) + planType
        local offset = 24 - (slot * 8)
        local mask = bit.bnot(bit.lshift(0xFF, offset))
        -- Lua bit.bnot is 32-bit; band with carePlan
        local zerod = bit.band(carePlan, mask)
        return bit.bor(zerod, bit.lshift(careByte, offset))
    end

    local function packOption(slot, length, planType)
        return bit.lshift(slot, 8) + bit.lshift(length, 16) + bit.lshift(planType, 19)
    end

    it('decodes slot length type from option', function()
        local opt = packOption(2, 5, 8)
        local d = decode(opt)
        assert(d.slot == 2 and d.length == 5 and d.type == 8)
        -- length is only 3 bits
        d = decode(packOption(0, 7, 12))
        assert(d.length == 7 and d.type == 12)
    end)

    it('default care plan is basic x7 all slots', function()
        local def = defaultCarePlan()
        -- each byte is 0x70 (len 7 type 0)
        assert(bit.band(bit.rshift(def, 24), 0xFF) == 0x70)
        assert(bit.band(bit.rshift(def, 16), 0xFF) == 0x70)
        assert(bit.band(bit.rshift(def, 8), 0xFF) == 0x70)
        assert(bit.band(def, 0xFF) == 0x70)
    end)

    it('applies slot update with zero default', function()
        local next = applySlot(0, 1, 3, 4) -- slot 1, length 3, type 4
        -- slot 1 is offset 16
        assert(bit.band(bit.rshift(next, 16), 0xFF) == bit.lshift(3, 4) + 4)
        -- other slots remain default 0x70
        assert(bit.band(bit.rshift(next, 24), 0xFF) == 0x70)
        assert(bit.band(bit.rshift(next, 8), 0xFF) == 0x70)
        assert(bit.band(next, 0xFF) == 0x70)
    end)

    it('overwrites single slot only', function()
        local base = defaultCarePlan()
        local next = applySlot(base, 0, 2, 5)
        assert(bit.band(bit.rshift(next, 24), 0xFF) == bit.lshift(2, 4) + 5)
        assert(bit.band(bit.rshift(next, 16), 0xFF) == 0x70)
    end)
end)
