-----------------------------------
-- Pure system tests for initChocoState report builder (slice 6168).
-----------------------------------

describe('chocobo init report pure plan', function()
    local BASIC_CARE = 0
    local RESTING = 1
    local EGG_HATCHING = 33
    local CHICK_TO_ADOLESCENT = 34
    local ADOLESCENT_TO_ADULT_1 = 35
    local ADULT_1_TO_ADULT_2 = 36
    local ADULT_2_TO_ADULT_3 = 37
    local ADULT_3_TO_ADULT_4 = 38
    local CRYING_AT_NIGHT = 69

    local DaysToChick = 4
    local DaysToAdolescent = 19
    local DaysToAdult1 = 29
    local DaysToAdult2 = 43
    local DaysToAdult3 = 64
    local DaysToAdult4 = 129

    local ageBoundaries =
    {
        { 1, DaysToChick,      EGG_HATCHING },
        { 2, DaysToAdolescent, CHICK_TO_ADOLESCENT },
        { 3, DaysToAdult1,     ADOLESCENT_TO_ADULT_1 },
        { 4, DaysToAdult2,     ADULT_1_TO_ADULT_2 },
        { 5, DaysToAdult3,     ADULT_2_TO_ADULT_3 },
        { 6, DaysToAdult4,     ADULT_3_TO_ADULT_4 },
    }

    local function ageToStage(age)
        for _, entry in ipairs(ageBoundaries) do
            if age <= entry[2] then
                return entry[1]
            end
        end
        return 7 -- ADULT_4
    end

    -- Expand care_plan bits into sequential day plan types (no zero-length default).
    local function expandCarePlanFuture(carePlan)
        local future = {}
        for i = 0, 3 do
            local offset   = 24 - (i * 8)
            local length   = bit.band(bit.rshift(carePlan, offset + 4), 0xF)
            local planType = bit.band(bit.rshift(carePlan, offset), 0xF)
            for _ = 1, length do
                future[#future + 1] = planType
            end
        end
        return future
    end

    local function ageUpCutscene(age)
        local currentStage = ageToStage(age)
        for _, entry in ipairs(ageBoundaries) do
            if currentStage == entry[1] and age >= entry[2] then
                return entry[3]
            end
        end
        return nil
    end

    -- Build raw report events for daysPassed = age - last_update_age (step 2 pure).
    -- Quest branch inject via optional questAtAge table: age → cutscene.
    local function planInitReportRaw(age, lastUpdateAge, carePlan, questAtAge)
        local daysPassed = age - lastUpdateAge
        if daysPassed <= 0 then
            return { earlyExit = true, daysPassed = daysPassed, events = {} }
        end

        local dayStart = lastUpdateAge
        local dayEnd   = age
        local reportLength = dayEnd - dayStart
        local future = expandCarePlanFuture(carePlan)
        local events = {}

        for idx = 1, reportLength do
            local planType = future[idx] or BASIC_CARE
            local dayAge   = dayStart + idx - 1
            events[#events + 1] = { dayAge, { planType } }

            local cs = ageUpCutscene(dayAge)
            if cs then
                events[#events + 1] = { dayAge, { cs } }
            end

            if questAtAge and questAtAge[dayAge] then
                events[#events + 1] = { dayAge, { questAtAge[dayAge] } }
            end
        end

        return {
            earlyExit = false,
            daysPassed = daysPassed,
            dayStart = dayStart,
            dayEnd = dayEnd,
            reportLength = reportLength,
            events = events,
            lastUpdateAge = age,
        }
    end

    it('expands default care plan into 28 basic-care days', function()
        local future = expandCarePlanFuture(0x70707070)
        assert(#future == 28)
        for i = 1, 28 do
            assert(future[i] == BASIC_CARE)
        end
    end)

    it('expands mixed slots without zero-length default', function()
        -- slot0: len=2 type=RESTING, rest zero → only 2 days
        local care = bit.lshift(bit.bor(bit.lshift(2, 4), RESTING), 24)
        local future = expandCarePlanFuture(care)
        assert(#future == 2 and future[1] == RESTING and future[2] == RESTING)
    end)

    it('age-up cutscenes fire on boundary days only', function()
        assert(ageUpCutscene(4) == EGG_HATCHING)
        assert(ageUpCutscene(3) == nil)
        assert(ageUpCutscene(5) == nil)
        assert(ageUpCutscene(19) == CHICK_TO_ADOLESCENT)
        assert(ageUpCutscene(29) == ADOLESCENT_TO_ADULT_1)
        assert(ageUpCutscene(43) == ADULT_1_TO_ADULT_2)
        assert(ageUpCutscene(64) == ADULT_2_TO_ADULT_3)
        assert(ageUpCutscene(129) == ADULT_3_TO_ADULT_4)
    end)

    it('early exits when no days passed', function()
        local r = planInitReportRaw(5, 5, 0x70707070, nil)
        assert(r.earlyExit and r.daysPassed == 0 and #r.events == 0)
        r = planInitReportRaw(4, 5, 0x70707070, nil)
        assert(r.earlyExit and r.daysPassed == -1)
    end)

    it('builds care + hatch events across egg boundary', function()
        -- last_update_age=1, age=5 → days 1..4 in report (reportLength=4)
        -- day ages: 1,2,3,4
        local r = planInitReportRaw(5, 1, 0x70707070, nil)
        assert(not r.earlyExit and r.reportLength == 4 and r.dayStart == 1 and r.dayEnd == 5)
        assert(r.lastUpdateAge == 5)
        -- 4 care events + hatch on day 4
        assert(#r.events == 5)
        assert(r.events[1][1] == 1 and r.events[1][2][1] == BASIC_CARE)
        assert(r.events[4][1] == 4 and r.events[4][2][1] == BASIC_CARE)
        assert(r.events[5][1] == 4 and r.events[5][2][1] == EGG_HATCHING)
    end)

    it('falls back to basic care past expanded schedule', function()
        local care = bit.lshift(bit.bor(bit.lshift(1, 4), RESTING), 24) -- 1 day RESTING
        local r = planInitReportRaw(4, 1, care, nil) -- ages 1,2,3
        assert(#r.events == 3)
        assert(r.events[1][2][1] == RESTING)
        assert(r.events[2][2][1] == BASIC_CARE)
        assert(r.events[3][2][1] == BASIC_CARE)
    end)

    it('inserts quest cutscene when provided for age', function()
        local r = planInitReportRaw(8, 7, 0x70707070, { [7] = CRYING_AT_NIGHT })
        -- day age 7 only (reportLength=1)
        assert(#r.events == 2)
        assert(r.events[1][2][1] == BASIC_CARE)
        assert(r.events[2][2][1] == CRYING_AT_NIGHT)
    end)
end)
