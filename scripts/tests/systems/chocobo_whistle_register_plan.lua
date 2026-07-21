-----------------------------------
-- Pure system tests for whistle game + register traits (slice 6166).
-----------------------------------

describe('chocobo whistle and register pure plan', function()
    local KI_HANDKERCHIEF = 68 -- may differ; inject only for pure

    -- WHISTLE_GAME_RESULT: roll < 25 success (note: < not <=)
    local function planWhistleGame(roll)
        if roll >= 1 and roll < 25 then
            return { success = true, prog = 3, getKi = 1 }
        end
        return { success = false, getKi = 2 }
    end

    local function registerTraits(strength, endurance, discernment)
        return {
            largeBeak   = discernment >= 128 and 1 or 0,
            fullTail    = endurance >= 128 and 1 or 0,
            largeTalons = strength >= 128 and 1 or 0,
        }
    end

    -- skip report: collapse all report events into csList and clear events
    local function planSkipReport(events)
        -- events: { { start, end, csList }, ... }
        local age, stage, csList = 0, 0, {}
        for _, ev in ipairs(events) do
            age = ev[1]
            -- stage from age handled by AgeToStage inject
            local duration = ev[2] - ev[1] + 1
            for _, cs in ipairs(ev[3]) do
                csList[#csList + 1] = { cs = cs, duration = duration }
            end
        end
        return { age = age, csCount = #csList, eventsCleared = true }
    end

    it('whistle game success under 25', function()
        assert(planWhistleGame(24).success and planWhistleGame(24).prog == 3)
        assert(planWhistleGame(1).success)
        assert(not planWhistleGame(25).success)
        assert(planWhistleGame(25).getKi == 2)
        assert(planWhistleGame(24).getKi == 1)
    end)

    it('register traits from stat thresholds', function()
        local t = registerTraits(128, 127, 128)
        assert(t.largeBeak == 1 and t.fullTail == 0 and t.largeTalons == 1)
        t = registerTraits(127, 128, 127)
        assert(t.largeBeak == 0 and t.fullTail == 1 and t.largeTalons == 0)
    end)

    it('skip report flattens events', function()
        local r = planSkipReport({
            { 1, 2, { 10, 11 } },
            { 5, 5, { 20 } },
        })
        assert(r.age == 5 and r.csCount == 3 and r.eventsCleared)
        r = planSkipReport({})
        assert(r.csCount == 0 and r.eventsCleared)
    end)
end)
