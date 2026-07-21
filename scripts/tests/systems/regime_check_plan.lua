-----------------------------------
-- Pure system tests for regime checkRegime pure plan (slice 6182).
-----------------------------------

describe('regime checkRegime pure plan', function()
    local FIELDS = 1
    local GROUNDS = 2

    -- party: 0 solo, 1 party, 2 alliance
    local function planCreditGate(p)
        if not p.alive or p.playerRegimeId ~= p.mobRegimeId then
            return false
        end
        if p.regimeType == FIELDS and not p.fovAllianceReward and p.partyType == 2 then
            return false
        end
        if p.regimeType == GROUNDS and not p.govAllianceReward and p.partyType == 2 then
            return false
        end
        if not p.killCredit then
            return false
        end
        return true
    end

    local function planKillIncrement(needed, killed)
        if killed == needed then
            return { done = true, killed = killed }
        end
        killed = killed + 1
        return { done = false, killed = killed, finishedSlot = needed <= killed }
    end

    local function planPageComplete(killed, needed)
        for i = 1, 4 do
            if (killed[i] or 0) < (needed[i] or 0) then
                return false
            end
        end
        return true
    end

    local function planLevelCapReward(reward, pageLow, pageHigh, capMin, capMax)
        if capMax > 0 and pageHigh > capMax then
            local avgCap = (capMin + capMax) / 2
            local avgMob = (pageLow + pageHigh) / 2
            return math.floor(reward * avgCap / avgMob)
        end
        return reward
    end

    local function planProwessReward(reward, govClears)
        -- reward * (100 + clears*4) / 100, clamp [0, 2*base]
        local base = reward
        reward = reward * (100 + (govClears * 4)) / 100
        if reward > base * 2 then
            reward = base * 2
        end
        if reward < 0 then
            reward = 0
        end
        return math.floor(reward)
    end

    it('credit gates', function()
        assert(planCreditGate({
            alive = true, playerRegimeId = 5, mobRegimeId = 5,
            regimeType = FIELDS, fovAllianceReward = false, partyType = 0, killCredit = true,
        }))
        assert(not planCreditGate({
            alive = false, playerRegimeId = 5, mobRegimeId = 5,
            regimeType = FIELDS, fovAllianceReward = true, partyType = 0, killCredit = true,
        }))
        assert(not planCreditGate({
            alive = true, playerRegimeId = 5, mobRegimeId = 6,
            regimeType = FIELDS, fovAllianceReward = true, partyType = 0, killCredit = true,
        }))
        assert(not planCreditGate({
            alive = true, playerRegimeId = 5, mobRegimeId = 5,
            regimeType = FIELDS, fovAllianceReward = false, partyType = 2, killCredit = true,
        }))
        assert(planCreditGate({
            alive = true, playerRegimeId = 5, mobRegimeId = 5,
            regimeType = FIELDS, fovAllianceReward = true, partyType = 2, killCredit = true,
        }))
        assert(not planCreditGate({
            alive = true, playerRegimeId = 5, mobRegimeId = 5,
            regimeType = GROUNDS, govAllianceReward = false, partyType = 2, killCredit = true,
        }))
        assert(not planCreditGate({
            alive = true, playerRegimeId = 5, mobRegimeId = 5,
            regimeType = FIELDS, fovAllianceReward = true, partyType = 0, killCredit = false,
        }))
    end)

    it('increments kills until slot done', function()
        local r = planKillIncrement(3, 3)
        assert(r.done and r.killed == 3)
        r = planKillIncrement(3, 1)
        assert(not r.done and r.killed == 2 and not r.finishedSlot)
        r = planKillIncrement(3, 2)
        assert(r.finishedSlot and r.killed == 3)
    end)

    it('detects page complete', function()
        assert(planPageComplete({ 1, 2, 0, 0 }, { 1, 2, 0, 0 }))
        assert(not planPageComplete({ 1, 1, 0, 0 }, { 1, 2, 0, 0 }))
    end)

    it('scales reward by level cap', function()
        assert(planLevelCapReward(100, 90, 100, 1, 80) == math.floor(100 * 40.5 / 95))
        assert(planLevelCapReward(100, 10, 20, 1, 0) == 100) -- cap disabled
        assert(planLevelCapReward(100, 10, 20, 1, 99) == 100) -- page under cap
    end)

    it('applies prowess clear bonus with 2x hard cap', function()
        -- 0 clears: no change (0*4)
        assert(planProwessReward(100, 0) == 100)
        -- 25 clears: +100% → 200, at hard cap
        assert(planProwessReward(100, 25) == 200)
        -- 50 clears: would be 300 → clamped 200
        assert(planProwessReward(100, 50) == 200)
    end)
end)
