-----------------------------------
-- Pure system tests for clamming zonikki NPC plan (slice 6149).
-----------------------------------

describe('clamming zonikki pure plan', function()
    local CSID_START  = 28
    local CSID_ACTIVE = 29
    local CSID_BROKEN = 30

    local function planTrigger(hasKit, kitBroken, oweItems)
        if hasKit then
            if kitBroken then
                return CSID_BROKEN
            end
            return CSID_ACTIVE
        end
        if oweItems then
            return -1 -- message + giveClammedItems (no event)
        end
        return CSID_START
    end

    local function enoughMoney(gil)
        return gil >= 500 and 1 or 2
    end

    local function canBuyKit(gil)
        return gil >= 500
    end

    -- Upgrade weight requirements by current size.
    local function canUpgrade(kitSize, kitWeight)
        if kitSize >= 200 then
            return false
        end
        if kitSize == 150 and kitWeight < 145 then
            return false
        end
        if kitSize == 100 and kitWeight < 95 then
            return false
        end
        if kitSize == 50 and kitWeight < 45 then
            return false
        end
        return true
    end

    local function upgradeSize(kitSize)
        return kitSize + 50
    end

    -- Finish action enum
    local ACT_NONE      = 0
    local ACT_BUY       = 1
    local ACT_RETURN    = 2
    local ACT_UPGRADE   = 3
    local ACT_BROKEN    = 4

    local function planFinish(csid, option, gil, kitSize, kitWeight)
        if csid == 28 and option == 1 then
            if not canBuyKit(gil) then
                return ACT_NONE
            end
            return ACT_BUY
        end
        if csid == 29 and option == 2 then
            return ACT_RETURN
        end
        if csid == 29 and option == 3 then
            if not canUpgrade(kitSize, kitWeight) then
                return ACT_NONE
            end
            return ACT_UPGRADE
        end
        if csid == 30 then
            return ACT_BROKEN
        end
        return ACT_NONE
    end

    it('trigger csid by kit state', function()
        assert(planTrigger(true, true, false) == CSID_BROKEN)
        assert(planTrigger(true, false, false) == CSID_ACTIVE)
        assert(planTrigger(false, false, true) == -1)
        assert(planTrigger(false, false, false) == CSID_START)
    end)

    it('buy kit gil gate', function()
        assert(enoughMoney(500) == 1 and enoughMoney(499) == 2)
        assert(canBuyKit(500) and not canBuyKit(499))
        assert(planFinish(28, 1, 500, 0, 0) == ACT_BUY)
        assert(planFinish(28, 1, 499, 0, 0) == ACT_NONE)
    end)

    it('upgrade weight requirements', function()
        assert(not canUpgrade(200, 999))
        assert(not canUpgrade(150, 144) and canUpgrade(150, 145))
        assert(not canUpgrade(100, 94) and canUpgrade(100, 95))
        assert(not canUpgrade(50, 44) and canUpgrade(50, 45))
        assert(upgradeSize(50) == 100 and upgradeSize(150) == 200)
        assert(planFinish(29, 3, 0, 50, 45) == ACT_UPGRADE)
        assert(planFinish(29, 3, 0, 50, 44) == ACT_NONE)
        assert(planFinish(29, 3, 0, 200, 200) == ACT_NONE)
    end)

    it('return and broken actions', function()
        assert(planFinish(29, 2, 0, 50, 10) == ACT_RETURN)
        assert(planFinish(30, 0, 0, 0, 0) == ACT_BROKEN)
        assert(planFinish(29, 1, 0, 50, 10) == ACT_NONE)
    end)
end)
