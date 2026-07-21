-----------------------------------
-- Pure system tests for compete + care schedule menu (slice 6164).
-----------------------------------

describe('chocobo compete and care schedule pure plan', function()
    local WIN_PLAYER, WIN_TIE, WIN_RIVAL = 0, 1, 2
    local EGG, CHICK, ADOLESCENT, ADULT_1 = 1, 2, 3, 4

    -- specialRoll 1..100: <=5 force tie; else sidePick is 0 or 2
    local function planCompete(specialRoll, sidePick)
        if specialRoll >= 1 and specialRoll <= 5 then
            return WIN_TIE
        end
        if sidePick == 0 or sidePick == 2 then
            return sidePick
        end
        return WIN_PLAYER
    end

    local function unpackCarePlan(carePlan)
        local plans = {}
        for i = 0, 3 do
            local offset = 24 - (i * 8)
            local length = bit.band(bit.rshift(carePlan, offset + 4), 0xF)
            local planType = bit.band(bit.rshift(carePlan, offset), 0xF)
            if length == 0 then
                length = 7
                planType = 0
            end
            plans[#plans + 1] = { length = length, type = planType }
        end
        return plans
    end

    local function packPlanInfo(plans)
        local planInfo = 0
        for i = 0, 3 do
            local p = plans[i + 1]
            local base = i * 8
            planInfo = planInfo + bit.lshift(p.length, base) + bit.lshift(p.type, base + 3)
        end
        return planInfo
    end

    local function planScheduleMenuMask(stage)
        local menuMask = 0x7FFFFFFF + (-bit.lshift(1, 0)) -- basic always
        if stage >= CHICK then
            for b = 1, 3 do
                menuMask = menuMask + (-bit.lshift(1, b))
            end
        end
        if stage >= ADOLESCENT then
            for b = 4, 9 do
                menuMask = menuMask + (-bit.lshift(1, b))
            end
        end
        if stage >= ADULT_1 then
            for b = 10, 12 do
                menuMask = menuMask + (-bit.lshift(1, b))
            end
        end
        return menuMask
    end

    local function bitClear(flags, n)
        return bit.band(flags, bit.lshift(1, n)) == 0
    end

    it('compete winner tie and sides', function()
        assert(planCompete(5, 0) == WIN_TIE)
        assert(planCompete(1, 2) == WIN_TIE)
        assert(planCompete(6, 0) == WIN_PLAYER)
        assert(planCompete(100, 2) == WIN_RIVAL)
    end)

    it('packs care schedule planInfo from care_plan', function()
        -- slot0 len=3 type=2 at high byte: length bits 28-31, type 24-27
        local care = bit.bor(bit.lshift(3, 28), bit.lshift(2, 24))
        care = bit.bor(care, bit.lshift(5, 20), bit.lshift(1, 16))
        care = bit.bor(care, bit.lshift(7, 12), bit.lshift(0, 8))
        care = bit.bor(care, bit.lshift(4, 4), bit.lshift(8, 0))
        local plans = unpackCarePlan(care)
        assert(plans[1].length == 3 and plans[1].type == 2)
        local info = packPlanInfo(plans)
        -- plan1: length at 0, type at 3
        assert(bit.band(info, 0x7) == 3)
        assert(bit.band(bit.rshift(info, 3), 0xF) == 2)
        assert(bit.band(bit.rshift(info, 8), 0x7) == 5)
        assert(bit.band(bit.rshift(info, 11), 0xF) == 1)
    end)

    it('care schedule menu mask by stage', function()
        local egg = planScheduleMenuMask(EGG)
        assert(bitClear(egg, 0) and not bitClear(egg, 1))
        local chick = planScheduleMenuMask(CHICK)
        assert(bitClear(chick, 1) and bitClear(chick, 3) and not bitClear(chick, 4))
        local ado = planScheduleMenuMask(ADOLESCENT)
        assert(bitClear(ado, 4) and bitClear(ado, 9) and not bitClear(ado, 10))
        local adult = planScheduleMenuMask(ADULT_1)
        assert(bitClear(adult, 10) and bitClear(adult, 12))
    end)
end)
