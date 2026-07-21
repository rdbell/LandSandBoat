-----------------------------------
-- Pure system tests for conquest crystal donate + exp ring recharge (slice 6207).
-----------------------------------

describe('conquest crystal pure plan', function()
    local CITY, FOREIGN, OUTPOST, BORDER = 1, 2, 3, 4
    local CAP = 4000

    local crystals =
    {
        [4096] = 12, -- FIRE
        [4097] = 12, -- ICE
        [4098] = 12, -- WIND
        [4099] = 12, -- EARTH
        [4100] = 12, -- LIGHTNING
        [4101] = 12, -- WATER
        [4102] = 16, -- LIGHT
        [4103] = 16, -- DARK
        [4238] = 12, -- INFERNO
        [4239] = 12, -- GLACIER
        [4240] = 12, -- CYCLONE
        [4241] = 12, -- TERRA
        [4242] = 12, -- PLASMA
        [4243] = 12, -- TORRENT
        [4244] = 16, -- AURORA
        [4245] = 16, -- TWILIGHT
    }

    local expRings =
    {
        [15761] = { chargesWhenFull = 7, costPerCharge = 50  }, -- CHARIOT
        [15762] = { chargesWhenFull = 7, costPerCharge = 100 }, -- EMPRESS
        [15763] = { chargesWhenFull = 3, costPerCharge = 200 }, -- EMPEROR
    }

    local function crystalPoints(rank, crystalWorth, count)
        if count <= 0 then return 0 end
        local denom = rank * 12 - crystalWorth
        if denom <= 0 then return 0 end
        return count * math.floor(CAP / denom)
    end

    local function crystalDonate(rank, rankPoints, addPoints)
        if rank == 1 then
            return 'reject_rank1', rankPoints, 0
        end
        if rankPoints == CAP then
            return 'reject_full', rankPoints, 0
        end
        if addPoints <= 0 then
            return 'none', rankPoints, 0
        end
        if rankPoints + addPoints >= CAP then
            return 'overflow', CAP, rankPoints + addPoints - CAP
        end
        return 'add', rankPoints + addPoints, 0
    end

    local function expRingCost(full, costPer, left)
        if left < 0 then left = 0 end
        if left > full then left = full end
        local used = full - left
        if used == 0 then
            return true, 0, 0 -- alreadyFull, cost, used
        end
        return false, used * costPer, used
    end

    it('crystals catalog size and worth pins', function()
        local n = 0
        for _ in pairs(crystals) do n = n + 1 end
        assert(n == 16)
        assert(crystals[4096] == 12 and crystals[4102] == 16)
        assert(crystals[4238] == 12 and crystals[4245] == 16)
    end)

    it('expRings catalog pins', function()
        assert(expRings[15761].chargesWhenFull == 7 and expRings[15761].costPerCharge == 50)
        assert(expRings[15762].costPerCharge == 100)
        assert(expRings[15763].chargesWhenFull == 3 and expRings[15763].costPerCharge == 200)
    end)

    it('crystal points formula', function()
        assert(crystalPoints(2, 12, 2) == 666)
        assert(crystalPoints(5, 16, 1) == 90)
        assert(crystalPoints(1, 12, 1) == 0) -- denom 0
        assert(crystalPoints(2, 12, 0) == 0)
    end)

    it('crystal donate outcomes', function()
        local k, rp, cp = crystalDonate(1, 0, 100)
        assert(k == 'reject_rank1')
        k, rp, cp = crystalDonate(5, CAP, 100)
        assert(k == 'reject_full')
        k, rp, cp = crystalDonate(5, 1000, 0)
        assert(k == 'none')
        k, rp, cp = crystalDonate(5, 1000, 500)
        assert(k == 'add' and rp == 1500 and cp == 0)
        k, rp, cp = crystalDonate(5, 3900, 200)
        assert(k == 'overflow' and rp == CAP and cp == 100)
    end)

    it('can donate gate: own/other and city/foreign only', function()
        local function can(ownOrOther, guardType)
            return ownOrOther and guardType <= FOREIGN
        end
        assert(can(true, CITY) and can(true, FOREIGN))
        assert(not can(true, OUTPOST) and not can(true, BORDER))
        assert(not can(false, CITY))
    end)

    it('exp ring recharge cost', function()
        local full, cost, used = expRingCost(7, 50, 7)
        assert(full and cost == 0)
        full, cost, used = expRingCost(7, 50, 3)
        assert(not full and used == 4 and cost == 200)
        full, cost, used = expRingCost(3, 200, 0)
        assert(not full and used == 3 and cost == 600)
        full, cost, used = expRingCost(7, 100, 99)
        assert(full)
        full, cost, used = expRingCost(7, 50, -1)
        assert(not full and used == 7 and cost == 350)
    end)
end)
