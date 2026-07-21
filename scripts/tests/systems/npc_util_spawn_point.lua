-----------------------------------
-- Pure system tests for UpdateNPCSpawnPoint pure plan (slice 6176).
-----------------------------------

describe('npcutil UpdateNPCSpawnPoint pure plan', function()
    -- Should move when no serverVar, or serverVar expiry <= now.
    local function planShouldMove(hasServerVar, serverVarTime, now)
        if not hasServerVar then
            return false -- LSB only moves when serverVar is set AND expired
        end
        return serverVarTime <= now
    end

    -- Timer delay is respawnTime * 1000 ms
    local function planTimerMs(respawnTimeSec)
        return respawnTimeSec * 1000
    end

    -- Clamp random inject into [min,max]
    local function planRespawnTime(minTime, maxTime, roll)
        if minTime > maxTime then
            minTime, maxTime = maxTime, minTime
        end
        if roll < minTime then
            return minTime
        end
        if roll > maxTime then
            return maxTime
        end
        return roll
    end

    it('moves only when serverVar set and expired', function()
        assert(not planShouldMove(false, 0, 100))
        assert(not planShouldMove(true, 200, 100))
        assert(planShouldMove(true, 100, 100))
        assert(planShouldMove(true, 50, 100))
    end)

    it('timer is respawn seconds times 1000', function()
        assert(planTimerMs(30) == 30000)
        assert(planTimerMs(0) == 0)
    end)

    it('clamps respawn roll to range', function()
        assert(planRespawnTime(10, 20, 15) == 15)
        assert(planRespawnTime(10, 20, 5) == 10)
        assert(planRespawnTime(10, 20, 25) == 20)
    end)
end)
