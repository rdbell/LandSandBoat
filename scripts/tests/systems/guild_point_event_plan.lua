-----------------------------------
-- Pure system tests for guild point event plans (slice 6150).
-----------------------------------

describe('guild point event pure plan', function()
    local CAT_CRYSTAL = 0
    local CAT_ITEM_LO = 1
    local CAT_ITEM_HI = 2
    local CAT_KI      = 3
    local CANCEL      = bit.lshift(1, 30)

    local function category(option)
        return bit.band(bit.rshift(option, 2), 3)
    end

    local function kiIndex(option)
        return bit.band(bit.rshift(option, 5), 15) - 1
    end

    local function crystalIndex(option)
        return bit.band(bit.rshift(option, 5), 15)
    end

    local function itemIndex(option)
        local cat = category(option)
        return (cat - 1) * 4 + bit.band(option, 3)
    end

    local function itemQty(option)
        local q = bit.rshift(option, 9)
        if q > 12 then return 12 end
        if q < 0 then return 0 end
        return q
    end

    local function crystalQty(option)
        local q = bit.rshift(option, 9)
        if q < 0 then return 0 end
        return q
    end

    -- Trade: currentGuild var is guildId+1 when contracted
    local function tradeEligible(currentGuildVar, guildId, remainingPoints)
        if currentGuildVar - 1 ~= guildId then
            return { eligible = false }
        end
        if remainingPoints == 0 then
            return { eligible = true, noMore = true }
        end
        return { eligible = true, noMore = false }
    end

    -- Contract finish: option == -1 (as signed) and rank >= 3
    local function planContract(option, rank, oldGuild)
        -- bit.tobit(option) == -1
        if option ~= -1 and option ~= 0xFFFFFFFF then
            return { contract = false }
        end
        if rank < 3 then
            return { contract = false }
        end
        local isNew = (oldGuild == -1)
        return {
            contract   = true,
            newContract = isNew,
            terminate  = not isNew,
            restrictDaily = not isNew,
        }
    end

    -- Purchase plan (currency gates only; no entity)
    local function planUpdate(option, rank, currency, kiOffers, itemOffers, crystals)
        local cat = category(option)
        if cat == CAT_KI then
            local idx = kiIndex(option)
            local offer = kiOffers[idx]
            if not offer or rank < offer.rank then
                return { path = 'ki', buy = false, index = idx }
            end
            local buy = currency >= offer.cost
            return { path = 'ki', buy = buy, index = idx, id = offer.id, cost = offer.cost }
        end
        if cat == CAT_ITEM_LO or cat == CAT_ITEM_HI then
            local idx = itemIndex(option)
            local qty = itemQty(option)
            local offer = itemOffers[idx]
            if not offer or rank < offer.rank or qty <= 0 then
                return { path = 'item', buy = false, index = idx, qty = qty }
            end
            local cost = qty * offer.cost
            return {
                path = 'item', buy = currency >= cost, index = idx, id = offer.id,
                unitCost = offer.cost, qty = qty, cost = cost,
            }
        end
        if cat == CAT_CRYSTAL and option ~= CANCEL then
            local idx = crystalIndex(option)
            local qty = crystalQty(option)
            local cry = crystals[idx]
            if not cry or rank < 3 or qty <= 0 then
                return { path = 'crystal', buy = false, index = idx, qty = qty }
            end
            local cost = qty * cry.cost
            return {
                path = 'crystal', buy = currency >= cost, index = idx, id = cry.id,
                unitCost = cry.cost, qty = qty, cost = cost,
            }
        end
        return { path = 'none' }
    end

    local kiOffers = {
        [0] = { id = 100, rank = 3, cost = 30000 },
        [1] = { id = 101, rank = 8, cost = 95000 },
    }
    local itemOffers = {
        [0] = { id = 200, rank = 4, cost = 10000 },
        [5] = { id = 205, rank = 3, cost = 5000 },
    }
    local crystals = {
        [0] = { id = 1, cost = 1500 },
        [1] = { id = 2, cost = 200 },
    }

    it('trade eligibility by contract and remaining points', function()
        assert(tradeEligible(3, 2, 10).eligible == true and not tradeEligible(3, 2, 10).noMore)
        assert(tradeEligible(3, 2, 0).noMore == true)
        assert(tradeEligible(1, 2, 10).eligible == false)
    end)

    it('contract finish rank and old guild', function()
        local r = planContract(-1, 3, -1)
        assert(r.contract and r.newContract and not r.terminate)
        r = planContract(-1, 3, 1)
        assert(r.contract and r.terminate and r.restrictDaily)
        assert(planContract(-1, 2, -1).contract == false)
        assert(planContract(0, 3, -1).contract == false)
    end)

    it('key item purchase path', function()
        -- category 3, ki index 0: ((0+1)<<5) | (3<<2) = 32 | 12 = 44
        local option = bit.bor(bit.lshift(1, 5), bit.lshift(3, 2))
        assert(category(option) == CAT_KI and kiIndex(option) == 0)
        local r = planUpdate(option, 3, 30000, kiOffers, itemOffers, crystals)
        assert(r.path == 'ki' and r.buy and r.id == 100 and r.cost == 30000)
        r = planUpdate(option, 3, 29999, kiOffers, itemOffers, crystals)
        assert(r.buy == false)
        r = planUpdate(option, 2, 99999, kiOffers, itemOffers, crystals)
        assert(r.buy == false)
    end)

    it('item and crystal purchase paths', function()
        -- item low index 0 qty 2: (2<<9) | (1<<2) = 1024 | 4 = 1028
        local option = bit.bor(bit.lshift(2, 9), bit.lshift(CAT_ITEM_LO, 2))
        assert(itemIndex(option) == 0 and itemQty(option) == 2)
        local r = planUpdate(option, 4, 20000, kiOffers, itemOffers, crystals)
        assert(r.path == 'item' and r.buy and r.cost == 20000 and r.id == 200)
        -- crystal index 1 qty 3: (3<<9) | (1<<5) = 1536 | 32
        option = bit.bor(bit.lshift(3, 9), bit.lshift(1, 5))
        r = planUpdate(option, 3, 600, kiOffers, itemOffers, crystals)
        assert(r.path == 'crystal' and r.buy and r.id == 2 and r.cost == 600)
        r = planUpdate(option, 2, 9999, kiOffers, itemOffers, crystals)
        assert(r.buy == false)
        assert(planUpdate(CANCEL, 3, 9999, kiOffers, itemOffers, crystals).path == 'none')
    end)
end)
