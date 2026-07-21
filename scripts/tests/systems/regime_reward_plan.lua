-----------------------------------
-- Pure system tests for regime checkRegime reward pure plans (slice 6185).
-----------------------------------

describe('regime checkRegime reward pure plan', function()
    local function clamp(v, lo, hi)
        if v < lo then
            return lo
        end
        if v > hi then
            return hi
        end
        return v
    end

    -- tabs from gil reward; retail caps valor_point at 50000
    local function planTabs(reward, tabsRate, currentTabs)
        local tabs = math.floor(reward / 10) * tabsRate
        return clamp(tabs, 0, 50000 - currentTabs)
    end

    -- award gil/tabs once per Vanadiel day unless REGIME_WAIT == 0
    local function planCanDailyReward(regimeWait, lastReward, vanadielEpoch)
        return regimeWait == 0 or lastReward < vanadielEpoch
    end

    -- EXP if mainLvl >= max(1, pageLow - threshold)
    local function planExpEligible(mainLvl, pageLow, threshold)
        local minLvl = pageLow - threshold
        if minLvl < 1 then
            minLvl = 1
        end
        return mainLvl >= minLvl
    end

    local function planExpAmount(reward, bookExpRate)
        return reward * bookExpRate
    end

    -- after page complete: repeat resets kills, else clear regime
    local AFTER_REPEAT = 1
    local AFTER_CLEAR = 2

    local function planAfterComplete(regimeRepeat)
        if regimeRepeat == 1 then
            return AFTER_REPEAT
        end
        return AFTER_CLEAR
    end

    -- PROWESS clear counter: has effect → power+1 else start at 1
    local function planProwessClearPower(hasProwess, govClears)
        if hasProwess then
            return govClears + 1
        end
        return 1
    end

    it('computes tabs with cap', function()
        assert(planTabs(100, 1, 0) == 10)
        assert(planTabs(100, 2, 0) == 20)
        -- near cap
        assert(planTabs(1000, 1, 49995) == 5)
        assert(planTabs(1000, 1, 50000) == 0)
        assert(planTabs(5, 1, 0) == 0) -- floor(5/10)=0
    end)

    it('daily reward gate', function()
        assert(planCanDailyReward(0, 100, 50)) -- wait disabled
        assert(planCanDailyReward(1, 10, 20))
        assert(not planCanDailyReward(1, 20, 20))
        assert(not planCanDailyReward(1, 25, 20))
    end)

    it('exp eligibility and amount', function()
        assert(planExpEligible(10, 5, 0))
        assert(planExpEligible(5, 10, 5)) -- 10-5=5
        assert(not planExpEligible(4, 10, 5))
        assert(planExpEligible(1, 1, 99)) -- max(1, 1-99)=1
        assert(planExpAmount(100, 1.0) == 100)
        assert(planExpAmount(100, 1.5) == 150)
    end)

    it('after complete branch', function()
        assert(planAfterComplete(1) == AFTER_REPEAT)
        assert(planAfterComplete(0) == AFTER_CLEAR)
    end)

    it('prowess clear power', function()
        assert(planProwessClearPower(false, 0) == 1)
        assert(planProwessClearPower(true, 3) == 4)
    end)
end)
