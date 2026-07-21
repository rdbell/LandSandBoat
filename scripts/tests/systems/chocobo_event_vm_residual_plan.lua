-----------------------------------
-- Pure system tests for residual event_vm / trade pure plans (slice 6170).
-----------------------------------

describe('chocobo event_vm residual pure plan', function()
    local EGG = 1
    local ADULT_1 = 4
    local DIRTY_KI = 810

    -- Egg trade routing: no state → accept; wrong loc → reject(1); same loc → reject(0)
    local function planEggTrade(hasState, stateLoc, zoneLoc)
        if not hasState then
            return { accept = true, rejectArg = nil }
        end
        if stateLoc ~= zoneLoc then
            return { accept = false, rejectArg = 1 }
        end
        return { accept = false, rejectArg = 0 }
    end

    local function isRaisingEgg(itemId)
        local eggs = { [2312] = true, [2314] = true, [2317] = true, [2318] = true, [2319] = true }
        return eggs[itemId] == true
    end

    -- GIVE_UP / RETIRE → retiring true
    local function planRetire()
        return { retiring = true }
    end

    -- UNKNOWN_252: hasReport + fixed 1s
    local function planUnknown252(eventCount, stage)
        local hasReport = 0
        if eventCount > 0 then
            hasReport = 0xFFFFFFFF
        end
        return { hasReport = hasReport, stage = stage, ones = true }
    end

    -- DEBUG_ABILITIES_PRINT packed raw stats
    local function packDebugStats(str, endur, disc, recep)
        return bit.lshift(str, 0) + bit.lshift(endur, 8) + bit.lshift(disc, 16) + bit.lshift(recep, 24)
    end

    -- UNKNOWN_600 dirty handkerchief
    local function planUnknown600()
        return { ki = DIRTY_KI, getKi = 1 }
    end

    -- UNKNOWN_24672
    local function planUnknown24672(stage)
        return { arg1 = 20, stage = stage }
    end

    -- trade feed still-egg gate
    local function planStillEgg(stage)
        return stage == EGG
    end

    -- isTradeEvent flag: food given → 8 else 0
    local function planIsTradeEvent(foodCount)
        if foodCount and foodCount > 0 then
            return 8
        end
        return 0
    end

    -- White handkerchief playout KI ops
    local function planHandkerchiefCS(cs)
        if cs == 69 then -- CRYING_AT_NIGHT
            return { addKI = 806, delKI = false, prog = nil }
        elseif cs == 54 then -- THAT_SHOULD_BE_ENOUGH
            return { addKI = nil, delKI = true, prog = 1 }
        elseif cs == 53 then -- HAVENT_SEEN_YOU
            return { addKI = nil, delKI = true, prog = nil }
        end
        return nil
    end

    it('routes egg trade accept and reject', function()
        assert(planEggTrade(false, 0, 1).accept)
        assert(planEggTrade(true, 1, 2).rejectArg == 1)
        assert(planEggTrade(true, 2, 2).rejectArg == 0)
        assert(not planEggTrade(true, 2, 2).accept)
    end)

    it('identifies raising egg items', function()
        assert(isRaisingEgg(2312) and isRaisingEgg(2319))
        assert(not isRaisingEgg(1))
    end)

    it('sets retiring on give-up and retire', function()
        assert(planRetire().retiring)
    end)

    it('unknown 252 report flag', function()
        local r = planUnknown252(0, ADULT_1)
        assert(r.hasReport == 0 and r.stage == ADULT_1 and r.ones)
        r = planUnknown252(2, EGG)
        assert(r.hasReport == 0xFFFFFFFF)
    end)

    it('packs debug ability stats', function()
        local p = packDebugStats(1, 2, 3, 4)
        assert(bit.band(p, 0xFF) == 1)
        assert(bit.band(bit.rshift(p, 8), 0xFF) == 2)
        assert(bit.band(bit.rshift(p, 16), 0xFF) == 3)
        assert(bit.band(bit.rshift(p, 24), 0xFF) == 4)
    end)

    it('unknown 600 grants dirty handkerchief', function()
        local r = planUnknown600()
        assert(r.ki == 810 and r.getKi == 1)
    end)

    it('unknown 24672 displays stage with arg1=20', function()
        local r = planUnknown24672(ADULT_1)
        assert(r.arg1 == 20 and r.stage == ADULT_1)
    end)

    it('still-egg and trade-event flags', function()
        assert(planStillEgg(EGG) and not planStillEgg(ADULT_1))
        assert(planIsTradeEvent(0) == 0 and planIsTradeEvent(3) == 8)
    end)

    it('handkerchief playout ki ops', function()
        local r = planHandkerchiefCS(69)
        assert(r.addKI == 806 and not r.delKI)
        r = planHandkerchiefCS(54)
        assert(r.delKI and r.prog == 1)
        r = planHandkerchiefCS(53)
        assert(r.delKI and r.prog == nil)
        assert(planHandkerchiefCS(0) == nil)
    end)
end)
