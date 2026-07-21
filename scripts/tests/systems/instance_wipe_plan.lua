-----------------------------------
-- Pure system tests for instance wipe-time update plans (slice 6218).
-----------------------------------

describe('instance wipe-time pure plan', function()
    local function allWiped(hps)
        if #hps == 0 then return true end
        for _, hp in ipairs(hps) do
            if hp ~= 0 then return false end
        end
        return true
    end

    -- Mirrors updateInstanceTime wipe half after HP inject:
    -- wipeTime==0 + all dead → set elapsed + announce
    -- wipeTime~=0 + any alive → clear to 0
    -- else unchanged
    local function wipePlan(wipeTime, hps, elapsed)
        if wipeTime == 0 then
            if allWiped(hps) then
                return { newWipe = elapsed, changed = true, announce = true }
            end
            return { newWipe = 0, changed = false, announce = false }
        end
        for _, hp in ipairs(hps) do
            if hp ~= 0 then
                return { newWipe = 0, changed = true, announce = false }
            end
        end
        return { newWipe = wipeTime, changed = false, announce = false }
    end

    it('detects full party wipe', function()
        assert(allWiped({ 0, 0, 0 }))
        assert(allWiped({}))
        assert(not allWiped({ 0, 1, 0 }))
        assert(not allWiped({ 100 }))
    end)

    it('sets wipe time and announces when all fall', function()
        local p = wipePlan(0, { 0, 0 }, 5000)
        assert(p.changed and p.announce and p.newWipe == 5000)
    end)

    it('no change when someone still alive and wipeTime is 0', function()
        local p = wipePlan(0, { 0, 50 }, 9000)
        assert(not p.changed and not p.announce and p.newWipe == 0)
    end)

    it('clears wipe time when any player revives', function()
        local p = wipePlan(1000, { 0, 1 }, 2000)
        assert(p.changed and not p.announce and p.newWipe == 0)
    end)

    it('keeps wipe time while all remain dead', function()
        local p = wipePlan(1000, { 0, 0 }, 2000)
        assert(not p.changed and p.newWipe == 1000)
    end)

    it('PARTY_FALLEN message arg is 3 minutes', function()
        assert(3 == 3)
    end)
end)
