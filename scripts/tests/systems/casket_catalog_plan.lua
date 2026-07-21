-----------------------------------
-- Pure system tests for casket catalog + take pure plans (slice 6189).
-----------------------------------

describe('casket catalog pure plan', function()
    local randomTable = { 1, 3, 1, 2, 1, 2, 1, 1, 3, 1, 2, 1 }

    local cs =
    {
        [0]  = 1000, [1]  = 1003, [2]  = 1006, [3]  = 1009, [4]  = 1012, [5]  = 1015,
        [6]  = 1018, [7]  = 1021, [8]  = 1024, [9]  = 1027, [10] = 1030, [11] = 1033,
        [12] = 1036, [13] = 1039, [14] = 1042, [15] = 1045, [16] = 1048
    }

    local multipleItems = {
        [1214] = true, -- stone arrowheads
        [1215] = true,
        [1211] = true,
        [1212] = true,
        [1213] = true,
        [1217] = true,
        [1222] = true,
        [1962] = true,
    }

    local splitZones = {
        [172] = true, [173] = true, [191] = true, [190] = true,
        [193] = true, [194] = true, [196] = true, [198] = true,
    }

    local function planItemCount(tableIndex)
        return randomTable[tableIndex]
    end

    local function planLockedEvent(chestOffset)
        return cs[chestOffset] + 2
    end

    local function planUnlockedEvent(chestOffset)
        return cs[chestOffset]
    end

    local function planItemQuantity(itemID)
        if multipleItems[itemID] then
            return 33
        end
        return 1
    end

    local function planUseHiTable(isSplitZone, mobLvl)
        return isSplitZone and mobLvl > 50
    end

    -- subOption: 2 or 0 → skip take
    local TAKE_SKIP = 0
    local TAKE_OK = 1
    local TAKE_EMPTY = 2
    local TAKE_NO_SPACE = 3
    local TAKE_HAS_TEMP = 4

    local function planTakeItem(subOption, itemID, freeSlots, despawned)
        if despawned then
            return TAKE_SKIP
        end
        if subOption == 2 or subOption == 0 then
            return TAKE_SKIP
        end
        if itemID == 0 then
            return TAKE_EMPTY
        end
        if freeSlots == 0 then
            return TAKE_NO_SPACE
        end
        return TAKE_OK
    end

    local function planTakeTemp(subOption, tempID, alreadyHas, despawned)
        if despawned then
            return TAKE_SKIP
        end
        if subOption == 2 or subOption == 0 then
            return TAKE_SKIP
        end
        if tempID == 0 then
            return TAKE_EMPTY
        end
        if alreadyHas then
            return TAKE_HAS_TEMP
        end
        return TAKE_OK
    end

    it('random table length and samples', function()
        assert(#randomTable == 12)
        assert(planItemCount(1) == 1 and planItemCount(2) == 3 and planItemCount(12) == 1)
    end)

    it('cs event offsets', function()
        assert(planUnlockedEvent(0) == 1000)
        assert(planLockedEvent(0) == 1002)
        assert(planLockedEvent(16) == 1050)
    end)

    it('quantity and split hi', function()
        assert(planItemQuantity(1214) == 33)
        assert(planItemQuantity(9999) == 1)
        assert(planUseHiTable(true, 51))
        assert(not planUseHiTable(true, 50))
        assert(not planUseHiTable(false, 99))
        assert(splitZones[172] and not splitZones[1])
    end)

    it('take item/temp gates', function()
        assert(planTakeItem(2, 100, 5, false) == TAKE_SKIP)
        assert(planTakeItem(1, 0, 5, false) == TAKE_EMPTY)
        assert(planTakeItem(1, 100, 0, false) == TAKE_NO_SPACE)
        assert(planTakeItem(1, 100, 1, false) == TAKE_OK)
        assert(planTakeItem(1, 100, 1, true) == TAKE_SKIP)
        assert(planTakeTemp(1, 50, true, false) == TAKE_HAS_TEMP)
        assert(planTakeTemp(1, 50, false, false) == TAKE_OK)
    end)
end)
