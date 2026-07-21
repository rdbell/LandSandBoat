-----------------------------------
-- Pure system tests for regime bookOnEventFinish residual pure plans (slice 6184).
-----------------------------------

describe('regime bookOnEventFinish residual pure plan', function()
    -- SHELL power/tier by main level
    local function planShellTier(mLvl)
        if mLvl < 37 then
            return 1055, 1
        elseif mLvl < 57 then
            return 1641, 2
        elseif mLvl < 68 then
            return 2188, 3
        elseif mLvl < 76 then
            return 2617, 4
        end
        return 2930, 5
    end

    -- ENHANCES_PROT_SHELL_RCVD: protect +2/tier, shell +39/tier
    local function planEnhanceBonus(basePower, tier, bonusPerTier, hasMod)
        if hasMod then
            return basePower + (bonusPerTier * tier)
        end
        return basePower
    end

    -- regimeRepeat bit → stored 0/1
    local function planNormalizeRepeat(hasRepeatBit)
        if hasRepeatBit then
            return 1
        end
        return 0
    end

    -- page registration product (charvars pure)
    local function planRegister(page, regimeType, zoneId, hasRepeatBit)
        if not page then
            return { ok = false }
        end
        return {
            ok = true,
            type = regimeType,
            zone = zoneId,
            id = page[8],
            repeatFlag = planNormalizeRepeat(hasRepeatBit),
            needed = { page[1], page[2], page[3], page[4] },
        }
    end

    -- finish branch after cost resolved (mirrors bookOnEventFinish gates)
    local FINISH_INVALID = 0
    local FINISH_CANNOT_AFFORD = 1
    local FINISH_FOOD_BLOCKED = 2
    local FINISH_SERVICE = 3
    local FINISH_REGISTER = 4
    local FINISH_PAGE_MISSING = 5

    local function planFinish(opt, cost, tabs, hasFoodEffect, hasSelectPage)
        if not opt then
            return FINISH_INVALID
        end
        if cost and cost > tabs then
            return FINISH_CANNOT_AFFORD
        end
        if opt.food and hasFoodEffect then
            return FINISH_FOOD_BLOCKED
        end
        if opt.act then
            return FINISH_SERVICE
        end
        if opt.page then
            if not hasSelectPage then
                return FINISH_PAGE_MISSING
            end
            return FINISH_REGISTER
        end
        return FINISH_INVALID
    end

    it('shell tier by level', function()
        local p, t = planShellTier(1)
        assert(p == 1055 and t == 1)
        p, t = planShellTier(37)
        assert(p == 1641 and t == 2)
        p, t = planShellTier(57)
        assert(p == 2188 and t == 3)
        p, t = planShellTier(68)
        assert(p == 2617 and t == 4)
        p, t = planShellTier(76)
        assert(p == 2930 and t == 5)
    end)

    it('enhance bonus on protect and shell', function()
        -- protect: bonus 2 * tier 3 = +6
        assert(planEnhanceBonus(90, 3, 2, true) == 96)
        assert(planEnhanceBonus(90, 3, 2, false) == 90)
        -- shell: bonus 39 * tier 2 = +78
        assert(planEnhanceBonus(1641, 2, 39, true) == 1719)
        assert(planEnhanceBonus(1641, 2, 39, false) == 1641)
    end)

    it('normalizes repeat flag', function()
        assert(planNormalizeRepeat(true) == 1)
        assert(planNormalizeRepeat(false) == 0)
    end)

    it('registers page vars', function()
        local page = { 6, 1, 0, 0, 4, 8, 330, 56 }
        local r = planRegister(page, 1, 100, true)
        assert(r.ok and r.type == 1 and r.zone == 100 and r.id == 56)
        assert(r.repeatFlag == 1)
        assert(r.needed[1] == 6 and r.needed[2] == 1 and r.needed[3] == 0 and r.needed[4] == 0)
        assert(not planRegister(nil, 1, 100, false).ok)
    end)

    it('finish branch gates', function()
        assert(planFinish(nil, 0, 100, false, false) == FINISH_INVALID)
        assert(planFinish({ act = 'REGEN', cost = 20 }, 20, 19, false, false) == FINISH_CANNOT_AFFORD)
        assert(planFinish({ act = 'DRIED_MEAT', food = true }, 50, 100, true, false) == FINISH_FOOD_BLOCKED)
        assert(planFinish({ act = 'PROTECT' }, 15, 100, false, false) == FINISH_SERVICE)
        assert(planFinish({ page = 1 }, nil, 0, false, true) == FINISH_REGISTER)
        assert(planFinish({ page = 1 }, nil, 0, false, false) == FINISH_PAGE_MISSING)
    end)
end)
