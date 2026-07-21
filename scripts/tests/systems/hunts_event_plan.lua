-----------------------------------
-- Pure system tests for hunts event pure plans (slice 6180).
-----------------------------------

describe('hunts event pure plan', function()
    local STATUS_NONE = 0
    local STATUS_ACCEPTED = 1
    local STATUS_COMPLETED = 2
    local LOCK_BIT = bit.lshift(1, 24)
    local ACTIVE_MENU = 0x0002
    local COMPLETE_MENU = 0x000A
    local REGIME_MENU = 0x0001

    -- onTrigger scyldBits pure
    local function planTriggerBits(scyld, locked, huntStatus, huntId, regimeActive)
        local scyldBits = bit.lshift(scyld, 14)
        if locked then
            return scyldBits + LOCK_BIT
        end
        if huntStatus == STATUS_ACCEPTED then
            return scyldBits + bit.lshift(huntId, 4) + ACTIVE_MENU
        end
        if huntStatus == STATUS_COMPLETED then
            return scyldBits + bit.lshift(huntId, 4) + COMPLETE_MENU
        end
        if regimeActive then
            return scyldBits + REGIME_MENU
        end
        return scyldBits
    end

    -- finish action
    local FINISH_ACCEPT = 1
    local FINISH_CANCEL = 2
    local FINISH_CANCEL_REGIME = 3
    local FINISH_COMPLETE = 4
    local FINISH_NONE = 0

    local function planFinish(option, hunts)
        local huntId = bit.rshift(option, 3)
        if hunts[huntId] then
            return { action = FINISH_ACCEPT, huntId = huntId, fee = hunts[huntId].fee }
        end
        if option == 3 then
            return { action = FINISH_CANCEL }
        end
        if option == 4 then
            return { action = FINISH_CANCEL_REGIME }
        end
        if option == 5 then
            return { action = FINISH_COMPLETE }
        end
        return { action = FINISH_NONE }
    end

    local function planCompleteScyld(current, bounty)
        if current + bounty > 1000 then
            return 1000
        end
        return current + bounty
    end

    -- checkHunt credit: alive, in range, status accepted, matching id
    local function planCheckHunt(playerAlive, dist, status, playerHuntId, mobHuntId)
        if not playerAlive or dist > 100 then
            return { credit = false }
        end
        if status == STATUS_ACCEPTED and playerHuntId == mobHuntId then
            return { credit = true, newStatus = STATUS_COMPLETED }
        end
        return { credit = false }
    end

    it('packs trigger scyld bits', function()
        local b = planTriggerBits(10, false, 0, 0, false)
        assert(b == bit.lshift(10, 14))
        b = planTriggerBits(0, true, 0, 0, false)
        assert(bit.band(b, LOCK_BIT) ~= 0)
        b = planTriggerBits(0, false, STATUS_ACCEPTED, 5, false)
        assert(bit.band(b, ACTIVE_MENU) ~= 0)
        assert(bit.rshift(bit.band(b, 0xFFF0), 4) == 5)
        b = planTriggerBits(0, false, STATUS_COMPLETED, 7, false)
        assert(bit.band(b, COMPLETE_MENU) == COMPLETE_MENU)
        b = planTriggerBits(0, false, 0, 0, true)
        assert(bit.band(b, REGIME_MENU) ~= 0)
    end)

    it('routes finish options', function()
        local hunts = { [20] = { fee = 15, bounty = 50 } }
        -- option with huntId 20: 20 << 3 = 160
        local r = planFinish(160, hunts)
        assert(r.action == FINISH_ACCEPT and r.huntId == 20 and r.fee == 15)
        assert(planFinish(3, hunts).action == FINISH_CANCEL)
        assert(planFinish(4, hunts).action == FINISH_CANCEL_REGIME)
        assert(planFinish(5, hunts).action == FINISH_COMPLETE)
        assert(planFinish(0, hunts).action == FINISH_NONE)
    end)

    it('caps complete scyld', function()
        assert(planCompleteScyld(900, 50) == 950)
        assert(planCompleteScyld(980, 50) == 1000)
    end)

    it('credits checkHunt only when eligible', function()
        assert(planCheckHunt(true, 50, STATUS_ACCEPTED, 10, 10).credit)
        assert(planCheckHunt(true, 50, STATUS_ACCEPTED, 10, 10).newStatus == STATUS_COMPLETED)
        assert(not planCheckHunt(false, 50, STATUS_ACCEPTED, 10, 10).credit)
        assert(not planCheckHunt(true, 101, STATUS_ACCEPTED, 10, 10).credit)
        assert(not planCheckHunt(true, 50, STATUS_ACCEPTED, 10, 11).credit)
        assert(not planCheckHunt(true, 50, STATUS_NONE, 10, 10).credit)
    end)
end)
