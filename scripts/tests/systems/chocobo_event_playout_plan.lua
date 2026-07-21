-----------------------------------
-- Pure system tests for event_playout pure plans (slice 6169).
-----------------------------------

describe('chocobo event playout pure plan', function()
    local EGG = 1
    local CHICK = 2
    local ADOLESCENT = 3
    local ADULT_1 = 4
    local ADULT_2 = 5
    local ADULT_3 = 6
    local ADULT_4 = 7

    local EGG_HATCHING = 33
    local CHICK_TO_ADOLESCENT = 34
    local ADOLESCENT_TO_ADULT_1 = 35
    local ADULT_1_TO_ADULT_2 = 36
    local ADULT_2_TO_ADULT_3 = 37
    local ADULT_3_TO_ADULT_4 = 38
    local HANGS_HEAD = 51
    local COMPETE = 52
    local REPORT_BASIC = 0

    local stageMap =
    {
        [EGG_HATCHING] = CHICK,
        [CHICK_TO_ADOLESCENT] = ADOLESCENT,
        [ADOLESCENT_TO_ADULT_1] = ADULT_1,
        [ADULT_1_TO_ADULT_2] = ADULT_2,
        [ADULT_2_TO_ADULT_3] = ADULT_3,
        [ADULT_3_TO_ADULT_4] = ADULT_4,
    }

    -- PlanStageTransition: growth CS → new stage; adult2→3 may force rename.
    local function planStageTransition(cs, first, last)
        local stage = stageMap[cs]
        if not stage then
            return { ok = false }
        end
        local forceRename = false
        if cs == ADULT_2_TO_ADULT_3 and first == 'Chocobo' and last == 'Chocobo' then
            forceRename = true
        end
        return { ok = true, stage = stage, forceRename = forceRename }
    end

    -- handleStatChange with unit multipliers (pos=1,neg=1)
    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function handleStat(value, change, max, posMult, negMult)
        if change == 0 then return value end
        if change > 0 then
            change = change * posMult
        else
            change = change * negMult
        end
        return clamp(value + change, 0, max)
    end

    -- Scold (HANGS_HEAD_IN_SHAME): affection -10, energy -5, clear SPOILED
    local function planScold(affection, energy, conditions, posMult, negMult)
        return {
            affection = handleStat(affection, -10, 255, posMult, negMult),
            energy = handleStat(energy, -5, 100, posMult, negMult),
            conditions = bit.band(conditions, bit.bnot(bit.lshift(1, 4))), -- SPOILED=4
        }
    end

    -- Compete playout: affection +1, energy -5, clear BORED
    local function planCompete(affection, energy, conditions, posMult, negMult)
        return {
            affection = handleStat(affection, 1, 255, posMult, negMult),
            energy = handleStat(energy, -5, 100, posMult, negMult),
            conditions = bit.band(conditions, bit.bnot(bit.lshift(1, 5))), -- BORED=5
        }
    end

    -- handleCSUpdate pure: pop first csList entry, csToPlay = location*256 + offset
    local function planCSUpdate(csList, location)
        if #csList == 0 then
            return { ok = false, remaining = {} }
        end
        local entry = csList[1]
        local csOffset, elapsed
        if type(entry) == 'table' then
            csOffset = entry[1]
            elapsed = entry[2] or 1
        else
            csOffset = entry
            elapsed = 1
        end
        local remaining = {}
        for i = 2, #csList do
            remaining[#remaining + 1] = csList[i]
        end
        return {
            ok = true,
            csOffset = csOffset,
            elapsedDays = elapsed,
            csToPlay = location * 256 + csOffset,
            remaining = remaining,
        }
    end

    -- Care-plan report CS → care plan type (identity for 0..12)
    local function carePlanFromReportCS(cs)
        if cs >= 0 and cs <= 12 then
            return cs
        end
        return nil
    end

    it('maps growth cutscenes to stages', function()
        assert(planStageTransition(EGG_HATCHING).stage == CHICK)
        assert(planStageTransition(CHICK_TO_ADOLESCENT).stage == ADOLESCENT)
        assert(planStageTransition(ADULT_3_TO_ADULT_4).stage == ADULT_4)
        assert(not planStageTransition(REPORT_BASIC).ok)
    end)

    it('forces rename on adult2→3 with default name', function()
        local r = planStageTransition(ADULT_2_TO_ADULT_3, 'Chocobo', 'Chocobo')
        assert(r.ok and r.stage == ADULT_3 and r.forceRename)
        r = planStageTransition(ADULT_2_TO_ADULT_3, 'Bob', 'Chocobo')
        assert(r.ok and not r.forceRename)
    end)

    it('scold applies affection/energy and clears spoiled', function()
        local SPOILED = bit.lshift(1, 4)
        local r = planScold(100, 50, SPOILED, 1, 1)
        assert(r.affection == 90 and r.energy == 45 and r.conditions == 0)
        r = planScold(5, 3, 0, 1, 2) -- neg mult 2
        assert(r.affection == 0 and r.energy == 0) -- 5-20, 3-10 clamped
    end)

    it('compete playout applies affection/energy and clears bored', function()
        local BORED = bit.lshift(1, 5)
        local r = planCompete(10, 50, BORED, 1, 1)
        assert(r.affection == 11 and r.energy == 45 and r.conditions == 0)
    end)

    it('cs update pops list and packs location offset', function()
        local r = planCSUpdate({ { 33, 3 }, 10 }, 2)
        assert(r.ok and r.csOffset == 33 and r.elapsedDays == 3)
        assert(r.csToPlay == 2 * 256 + 33)
        assert(#r.remaining == 1 and r.remaining[1] == 10)
        r = planCSUpdate({}, 1)
        assert(not r.ok)
        r = planCSUpdate({ 5 }, 1)
        assert(r.csOffset == 5 and r.elapsedDays == 1 and r.csToPlay == 256 + 5)
    end)

    it('report care cutscenes map to care plans', function()
        assert(carePlanFromReportCS(0) == 0)
        assert(carePlanFromReportCS(12) == 12)
        assert(carePlanFromReportCS(33) == nil)
    end)
end)
