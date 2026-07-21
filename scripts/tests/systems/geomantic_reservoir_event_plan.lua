-----------------------------------
-- Pure system tests for geomantic reservoir event pure plans (slice 6191).
-----------------------------------

describe('geomantic reservoir event pure plan', function()
    local LEARN_CSID = 15000
    local PROC_MIN = 230
    local PROC_MAX = 300

    local function planTrigger(canLearn)
        if canLearn then
            return 'learn'
        end
        return 'nothing'
    end

    -- math.random(230, 300) inject: clamp into range for host roll
    local function planProcTime(roll)
        if roll < PROC_MIN then
            return PROC_MIN
        end
        if roll > PROC_MAX then
            return PROC_MAX
        end
        return roll
    end

    local function planFinish(csid)
        return csid == LEARN_CSID
    end

    it('trigger branch', function()
        assert(planTrigger(true) == 'learn')
        assert(planTrigger(false) == 'nothing')
    end)

    it('proc time range', function()
        assert(planProcTime(230) == 230)
        assert(planProcTime(300) == 300)
        assert(planProcTime(250) == 250)
        assert(planProcTime(100) == 230)
        assert(planProcTime(999) == 300)
    end)

    it('finish gate', function()
        assert(planFinish(15000))
        assert(not planFinish(0))
        assert(not planFinish(14999))
    end)
end)
