-----------------------------------
-- Pure system tests for condition menu + care-for + watch-over (slice 6165).
-----------------------------------

describe('chocobo condition care watch pure plan', function()
    local CHICK, ADOLESCENT, ADULT_1, EGG = 2, 3, 4, 1
    local INJURED, SICK, ILL, HIGH_SPIRITS = 3, 2, 0, 8
    local VERY_ILL, BORED, SPOILED, RUN_AWAY = 1, 5, 4, 7
    local LOVESICK, ENERGY1, ENERGY2, BRIGHT = 6, 11, 12, 13

    local function condBit(conditions, c)
        return bit.band(conditions, bit.lshift(1, c)) ~= 0
    end

    local function planArg4(conditions)
        local arg4 = 0
        if condBit(conditions, INJURED) then arg4 = arg4 + bit.lshift(1, 0) end
        if condBit(conditions, SICK) then arg4 = arg4 + bit.lshift(1, 1) end
        if condBit(conditions, ILL) then arg4 = arg4 + bit.lshift(1, 2) end
        if condBit(conditions, HIGH_SPIRITS) then arg4 = arg4 + bit.lshift(1, 4) end
        if condBit(conditions, VERY_ILL) then arg4 = arg4 + bit.lshift(1, 6) end
        if condBit(conditions, BORED) then arg4 = arg4 + bit.lshift(1, 7) end
        if condBit(conditions, SPOILED) then arg4 = arg4 + bit.lshift(1, 8) end
        if condBit(conditions, RUN_AWAY) then arg4 = arg4 + bit.lshift(1, 9) end
        if condBit(conditions, LOVESICK) then arg4 = arg4 + bit.lshift(1, 10) end
        if condBit(conditions, ENERGY1) or condBit(conditions, ENERGY2) then
            arg4 = arg4 + bit.lshift(1, 12)
        end
        if condBit(conditions, BRIGHT) then arg4 = arg4 + bit.lshift(1, 13) end
        return arg4
    end

    local function packArg2(affectionRank, hunger)
        return affectionRank + bit.lshift(hunger, 16)
    end

    local function packArg3(personality, weatherPref, ability1, ability2, stage)
        return personality +
            bit.lshift(weatherPref, 4) +
            bit.lshift(ability1, 8) +
            bit.lshift(ability2, 12) +
            bit.lshift(stage, 16)
    end

    local function careForMask(stage, knowsStory, hasRegularWalk)
        local mask = 0x7FFFFFFF + (-bit.lshift(1, 0)) -- watch over always
        if stage >= CHICK then
            mask = mask + (-bit.lshift(1, 2)) + (-bit.lshift(1, 4)) -- scold, short walk
        end
        if stage >= ADOLESCENT then
            if knowsStory then
                mask = mask + (-bit.lshift(1, 1)) -- story
            end
            mask = mask + (-bit.lshift(1, 5)) -- regular walk
            if hasRegularWalk then
                mask = mask + (-bit.lshift(1, 3)) -- compete
            end
        end
        if stage >= ADULT_1 then
            mask = mask + (-bit.lshift(1, 6)) -- long walk
        end
        return mask
    end

    local function bitClear(flags, n)
        return bit.band(flags, bit.lshift(1, n)) == 0
    end

    local WATCH_ENERGY = 5
    local function planWatch(stage, energy, heldItem, freeSlots)
        if stage == EGG then
            return { egg = true, badEggFlag = 0 }
        end
        local energyFlag = 0
        local newEnergy = energy
        if energy < WATCH_ENERGY then
            energyFlag = -1
        else
            newEnergy = energy - WATCH_ENERGY
        end
        local givingItem, givenItem = 0, 0
        if heldItem > 0 then
            givingItem, givenItem = 1, heldItem
            if freeSlots == 0 then
                givingItem = 2
            end
        end
        return {
            egg = false,
            energyFlag = energyFlag,
            newEnergy = newEnergy,
            givingItem = givingItem,
            givenItem = givenItem,
            clearHeld = (givingItem == 1),
        }
    end

    it('packs condition status flags', function()
        local c = bit.lshift(1, INJURED) + bit.lshift(1, ILL) + bit.lshift(1, ENERGY1)
        local a = planArg4(c)
        assert(bit.band(a, bit.lshift(1, 0)) ~= 0) -- leg wounded
        assert(bit.band(a, bit.lshift(1, 2)) ~= 0) -- stomach ache
        assert(bit.band(a, bit.lshift(1, 12)) ~= 0) -- full of energy
    end)

    it('packs affection hunger and personality args', function()
        assert(packArg2(3, 100) == 3 + bit.lshift(100, 16))
        local a3 = packArg3(2, 3, 1, 4, 5)
        assert(bit.band(a3, 0xF) == 2)
        assert(bit.band(bit.rshift(a3, 4), 0xF) == 3)
        assert(bit.band(bit.rshift(a3, 8), 0xF) == 1)
        assert(bit.band(bit.rshift(a3, 12), 0xF) == 4)
        assert(bit.band(bit.rshift(a3, 16), 0xF) == 5)
    end)

    it('care for menu mask by stage', function()
        local m = careForMask(EGG, true, true)
        assert(bitClear(m, 0) and not bitClear(m, 2))
        m = careForMask(CHICK, true, true)
        assert(bitClear(m, 2) and bitClear(m, 4) and not bitClear(m, 1))
        m = careForMask(ADOLESCENT, true, true)
        assert(bitClear(m, 1) and bitClear(m, 3) and bitClear(m, 5))
        m = careForMask(ADOLESCENT, false, false)
        assert(not bitClear(m, 1) and not bitClear(m, 3) and bitClear(m, 5))
        m = careForMask(ADULT_1, true, true)
        assert(bitClear(m, 6))
    end)

    it('watch over energy and item delivery', function()
        local r = planWatch(EGG, 10, 0, 5)
        assert(r.egg)
        r = planWatch(CHICK, 3, 0, 5)
        assert(r.energyFlag == -1 and r.newEnergy == 3)
        r = planWatch(CHICK, 10, 55, 5)
        assert(r.energyFlag == 0 and r.newEnergy == 5 and r.givingItem == 1 and r.clearHeld)
        r = planWatch(CHICK, 10, 55, 0)
        assert(r.givingItem == 2 and not r.clearHeld)
    end)
end)
