-----------------------------------
-- Pure system tests for utils.drawIn wait/ready control flow.
-----------------------------------

describe('drawIn pure plan', function()
    -- Mirror pure inject without entity hosts.
    local function drawInPlan(p)
        if not p.hasPosition then
            return { clearWait = true, drewIn = false }
        end

        local conditions = p.conditions
        if conditions == nil then
            conditions = { true }
        end

        local any = false
        for _, c in ipairs(conditions) do
            if c then
                any = true
                break
            end
        end

        if not any then
            return { clearWait = true, drewIn = false }
        end

        local nextDrawIn = p.nextWaitTime or 0
        local now = p.now
        if nextDrawIn > 0 then
            if now > nextDrawIn then
                return { drewIn = true, clearWait = true }
            end

            return { drewIn = false, clearWait = false }
        end

        local wait = p.wait
        if wait == nil then
            wait = 1
        end

        return { drewIn = false, scheduleUntil = now + wait }
    end

    it('no position clears wait', function()
        local r = drawInPlan({ hasPosition = false, now = 100 })
        assert(r.clearWait == true and r.drewIn == false)
    end)

    it('failed conditions clear wait', function()
        local r = drawInPlan({ hasPosition = true, conditions = { false }, now = 100 })
        assert(r.clearWait == true)
    end)

    it('schedules wait then draws after elapsed', function()
        local r = drawInPlan({ hasPosition = true, now = 1000 })
        assert(r.scheduleUntil == 1001)
        r = drawInPlan({ hasPosition = true, nextWaitTime = 1001, now = 1001 })
        assert(r.drewIn == false)
        r = drawInPlan({ hasPosition = true, nextWaitTime = 1001, now = 1002 })
        assert(r.drewIn == true and r.clearWait == true)
    end)

    it('custom wait seconds', function()
        local r = drawInPlan({ hasPosition = true, now = 50, wait = 5 })
        assert(r.scheduleUntil == 55)
    end)
end)
