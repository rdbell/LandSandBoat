-----------------------------------
-- Pure system tests for conquest vendor/teleporter pure plans (slice 6199).
-----------------------------------

describe('conquest vendor/teleporter pure plan', function()
    local KIND_NONE = 0
    local KIND_SHOP = 1
    local KIND_GIL_WARP = 2
    local KIND_CP_WARP = 3

    local function planVendorFinish(option)
        if option == 1 then
            return KIND_SHOP
        end
        if option == 2 then
            return KIND_GIL_WARP
        end
        if option == 6 then
            return KIND_CP_WARP
        end
        return KIND_NONE
    end

    local function planCanVendorGilWarp(gil, fee)
        return gil >= fee
    end

    local function planCanVendorCPWarp(cp, fee)
        return cp >= fee
    end

    -- teleporter update: region = option - 1073741829, cpFee = fee/10
    local function planTeleportUpdateRegion(option)
        return option - 1073741829
    end

    local function planCPFee(fee)
        return math.floor(fee / 10)
    end

    -- teleporter finish gil: option 5..23 → region = option-5
    local function planGilTeleportRegion(option)
        if option >= 5 and option <= 23 then
            return true, option - 5
        end
        return false, 0
    end

    -- teleporter finish CP: option 1029..1047 → region = option-1029
    local function planCPTeleportRegion(option)
        if option >= 1029 and option <= 1047 then
            return true, option - 1029
        end
        return false, 0
    end

    local function planCanTeleportGil(canOP, gil, fee)
        return canOP and gil >= fee
    end

    local function planCanTeleportCP(canOP, cp, cpFee)
        return canOP and cp >= cpFee
    end

    local function planNationBits(playerNation, teleporterNation)
        return playerNation + bit.lshift(teleporterNation, 8)
    end

    it('vendor finish branches', function()
        assert(planVendorFinish(1) == KIND_SHOP)
        assert(planVendorFinish(2) == KIND_GIL_WARP)
        assert(planVendorFinish(6) == KIND_CP_WARP)
        assert(planVendorFinish(0) == KIND_NONE)
        assert(planCanVendorGilWarp(100, 100) and not planCanVendorGilWarp(99, 100))
        assert(planCanVendorCPWarp(50, 50) and not planCanVendorCPWarp(49, 50))
    end)

    it('teleporter update and finish regions', function()
        assert(planTeleportUpdateRegion(1073741829) == 0)
        assert(planTeleportUpdateRegion(1073741834) == 5)
        assert(planCPFee(100) == 10)
        assert(planCPFee(150) == 15)
        local ok, r = planGilTeleportRegion(5)
        assert(ok and r == 0)
        ok, r = planGilTeleportRegion(23)
        assert(ok and r == 18)
        ok, r = planGilTeleportRegion(4)
        assert(not ok)
        ok, r = planCPTeleportRegion(1029)
        assert(ok and r == 0)
        ok, r = planCPTeleportRegion(1047)
        assert(ok and r == 18)
        ok, r = planCPTeleportRegion(1028)
        assert(not ok)
    end)

    it('teleport afford and nation bits', function()
        assert(planCanTeleportGil(true, 100, 100))
        assert(not planCanTeleportGil(false, 100, 100))
        assert(not planCanTeleportGil(true, 99, 100))
        assert(planCanTeleportCP(true, 10, 10))
        assert(not planCanTeleportCP(true, 9, 10))
        assert(planNationBits(0, 1) == 0 + bit.lshift(1, 8))
        assert(planNationBits(2, 0) == 2)
    end)
end)
