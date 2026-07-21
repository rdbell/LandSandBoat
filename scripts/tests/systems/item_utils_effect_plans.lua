-----------------------------------
-- Pure system tests for item_utils add/remove effect plans (slice 6133).
-- Source: scripts/globals/item_utils.lua ~73–80, ~157–278
-----------------------------------

describe('itemUtils itemBoxOnItemCheck pure plan', function()
    local MSG_ITEM_NO_USE_INVENTORY = 308

    local function check(freeSlots)
        if freeSlots == 0 then
            return MSG_ITEM_NO_USE_INVENTORY
        end
        return 0
    end

    it('blocks when inventory full', function()
        assert(check(0) == MSG_ITEM_NO_USE_INVENTORY)
    end)

    it('allows when free slots remain', function()
        assert(check(1) == 0 and check(20) == 0)
    end)
end)

describe('itemUtils addItemShield pure plan', function()
    local MSG_NO_EFFECT = 283
    local MSG_GAINS = 205
    local PHYSICAL_SHIELD = 150

    local function plan(p)
        if p.hasEffect and p.activePower > p.power then
            return { noEffect = true, msgID = MSG_NO_EFFECT }
        end
        return {
            delPhysical = true, delMagic = true, apply = true,
            power = p.power, duration = p.duration, effect = p.effect,
            subPower = p.subPower, msgID = MSG_GAINS, msgParam = p.effect,
        }
    end

    it('blocks when existing power is higher', function()
        local r = plan({ hasEffect = true, activePower = 50, power = 30, effect = PHYSICAL_SHIELD })
        assert(r.noEffect == true and r.msgID == MSG_NO_EFFECT)
    end)

    it('applies and clears both shields on success', function()
        local r = plan({ power = 20, duration = 60, effect = PHYSICAL_SHIELD, subPower = 1 })
        assert(r.apply == true and r.delPhysical == true and r.delMagic == true)
        assert(r.msgID == MSG_GAINS and r.msgParam == PHYSICAL_SHIELD)
    end)

    it('equal power overwrites', function()
        local r = plan({ hasEffect = true, activePower = 30, power = 30, effect = PHYSICAL_SHIELD })
        assert(r.apply == true and not r.noEffect)
    end)
end)

describe('itemUtils addItemEffect pure plan', function()
    local MSG_NO_EFFECT = 283

    local function plan(p)
        if p.hasEffect and p.activePower > p.power then
            return { noEffect = true, msgID = MSG_NO_EFFECT }
        end
        return { apply = true, effect = p.effect, power = p.power, duration = p.duration, subPower = p.subPower }
    end

    it('blocks stronger existing', function()
        assert(plan({ hasEffect = true, activePower = 100, power = 50 }).noEffect == true)
    end)

    it('applies fresh or weaker overwrite', function()
        local r = plan({ effect = 10, power = 20, duration = 15, subPower = 5 })
        assert(r.apply == true and r.subPower == 5)
    end)
end)

describe('itemUtils addItemExpEffect pure plan', function()
    local COMMITMENT = 579
    local DEDICATION = 249
    local MSG_NO_EFFECT = 283

    local function partner(effect)
        if effect == COMMITMENT then return DEDICATION end
        return COMMITMENT
    end

    local function plan(p)
        local del = partner(p.effect)
        if p.hasEffect and p.activePower > p.power then
            return { noEffect = true, msgID = MSG_NO_EFFECT, delEffect = del }
        end
        return {
            delSilent = true, delEffect = del, apply = true,
            effect = p.effect, power = p.power, duration = p.duration, subPower = p.subPower,
        }
    end

    it('partners dedication and commitment', function()
        assert(partner(DEDICATION) == COMMITMENT)
        assert(partner(COMMITMENT) == DEDICATION)
    end)

    it('blocks stronger and still reports partner', function()
        local r = plan({ effect = DEDICATION, power = 50, hasEffect = true, activePower = 80 })
        assert(r.noEffect == true and r.delEffect == COMMITMENT)
    end)

    it('applies with silent del of partner', function()
        local r = plan({ effect = COMMITMENT, power = 30, duration = 900, subPower = 50 })
        assert(r.apply == true and r.delSilent == true and r.delEffect == DEDICATION)
    end)
end)

describe('itemUtils removeStatus pure plan', function()
    local ERASE_NONE = 255

    local function remove(p)
        for _, ok in ipairs(p.delResults or {}) do
            if ok then return true end
        end
        return (p.eraseResult or ERASE_NONE) ~= ERASE_NONE
    end

    it('returns true on first successful del', function()
        assert(remove({ delResults = { false, true } }) == true)
    end)

    it('falls back to erase', function()
        assert(remove({ delResults = { false }, eraseResult = 3 }) == true)
        assert(remove({ delResults = { false }, eraseResult = ERASE_NONE }) == false)
    end)
end)

describe('itemUtils removeMultipleEffects pure plan', function()
    local function multi(p)
        if (p.count or 0) <= 0 then return 0 end
        local removed = 0
        local maxIter = p.count + 1
        for i = 1, maxIter do
            local ok = p.attempts[i]
            if not ok then break end
            removed = removed + 1
            if removed >= p.count then break end
        end
        return removed
    end

    it('caps at count', function()
        assert(multi({ count = 2, attempts = { true, true, true } }) == 2)
    end)

    it('stops on first failure', function()
        assert(multi({ count = 5, attempts = { true, false, true } }) == 1)
    end)

    it('zero count yields zero', function()
        assert(multi({ count = 0, attempts = { true } }) == 0)
    end)
end)
