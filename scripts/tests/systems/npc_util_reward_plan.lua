-----------------------------------
-- Pure system tests for giveReward / completeQuest / completeMission plans (slice 6174).
-----------------------------------

describe('npcutil reward pure plan', function()
    local DEFAULT_FAME = 30
    local RANK_CAP = 4000

    local function applyRate(amount, rate)
        return math.floor(amount * rate)
    end

    -- Normalize var param to list of names (string or table).
    local function normalizeVars(var)
        if var == nil then
            return {}
        end
        if type(var) == 'string' then
            return { var }
        end
        if type(var) == 'table' then
            local out = {}
            for _, v in ipairs(var) do
                out[#out + 1] = v
            end
            return out
        end
        return {}
    end

    -- giveReward pure: default fame=30; fame applies when fameArea set.
    local function planGiveReward(p)
        -- p: hasItem, itemOK, fame, fameAreaSet, gil, bayld, exp, gilRate, bayldRate, expRate, var
        if p.hasItem and not p.itemOK then
            return { ok = false }
        end
        local fame = p.fame
        if fame == nil then
            fame = DEFAULT_FAME
        end
        local out = {
            ok = true,
            applyFame = p.fameAreaSet == true,
            fame = fame,
            applyGil = p.gil ~= nil,
            gil = p.gil and applyRate(p.gil, p.gilRate or 1) or 0,
            applyBayld = p.bayld ~= nil,
            bayld = p.bayld and applyRate(p.bayld, p.bayldRate or 1) or 0,
            applyExp = p.exp ~= nil,
            exp = p.exp and applyRate(p.exp, p.expRate or 1) or 0,
            applyTitle = p.hasTitle == true,
            applyKeyItem = p.hasKeyItem == true,
            zeroVars = normalizeVars(p.var),
        }
        return out
    end

    -- completeQuest: fame only when fameAreaIsNumber; default fame inside that branch.
    local function planCompleteQuest(p)
        if p.hasItem and not p.itemOK then
            return { ok = false }
        end
        local applyFame = p.fameAreaIsNumber == true
        local fame = p.fame
        if applyFame and fame == nil then
            fame = DEFAULT_FAME
        end
        local logOk = p.logId ~= nil
        return {
            ok = true,
            applyFame = applyFame,
            fame = fame or 0,
            applyGil = p.gil ~= nil,
            gil = p.gil and applyRate(p.gil, p.gilRate or 1) or 0,
            completeQuest = logOk,
            zeroVars = normalizeVars(p.var),
        }
    end

    -- resolve log id: number or quest_log field
    local function resolveLogId(areaIsNumber, areaNumber, hasQuestLog, questLog)
        if areaIsNumber then
            return areaNumber, true
        end
        if hasQuestLog then
            return questLog, true
        end
        return nil, false
    end

    -- completeMission rank/next pure
    local function planMissionRank(currentRank, paramsRank, currentRP, addRP)
        local out = { setRank = false, rankPoints = currentRP, applyRP = false }
        if addRP ~= nil then
            out.applyRP = true
            out.rankPoints = math.min(currentRP + addRP, RANK_CAP)
        end
        if paramsRank ~= nil and currentRank < paramsRank then
            out.setRank = true
            out.newRank = paramsRank
            out.rankPoints = 0 -- cleared after setRank
        end
        return out
    end

    it('giveReward defaults fame and applies rates', function()
        local r = planGiveReward({ fameAreaSet = true, gil = 100, gilRate = 2, bayld = 50, bayldRate = 1.5, exp = 10, expRate = 2 })
        assert(r.ok and r.applyFame and r.fame == 30)
        assert(r.gil == 200 and r.bayld == 75 and r.exp == 20)
    end)

    it('giveReward fails when item cannot be given', function()
        local r = planGiveReward({ hasItem = true, itemOK = false })
        assert(not r.ok)
    end)

    it('giveReward skips fame without fameArea', function()
        local r = planGiveReward({ fame = 50, fameAreaSet = false })
        assert(r.ok and not r.applyFame and r.fame == 50)
    end)

    it('normalizes var to zero list', function()
        local r = planGiveReward({ var = 'foo' })
        assert(#r.zeroVars == 1 and r.zeroVars[1] == 'foo')
        r = planGiveReward({ var = { 'a', 'b' } })
        assert(#r.zeroVars == 2)
    end)

    it('completeQuest fame only with numeric fameArea', function()
        local r = planCompleteQuest({ fameAreaIsNumber = true, logId = 0 })
        assert(r.applyFame and r.fame == 30 and r.completeQuest)
        r = planCompleteQuest({ fameAreaIsNumber = false, fame = 99, logId = 1 })
        assert(not r.applyFame)
    end)

    it('resolves quest log id', function()
        local id, ok = resolveLogId(true, 5, false, 0)
        assert(ok and id == 5)
        id, ok = resolveLogId(false, 0, true, 7)
        assert(ok and id == 7)
        id, ok = resolveLogId(false, 0, false, 0)
        assert(not ok)
    end)

    it('mission rank points and rank gate', function()
        -- When rank increases, RP is clamped then cleared (final rankPoints = 0).
        local r = planMissionRank(5, 6, 3900, 200)
        assert(r.applyRP and r.setRank and r.newRank == 6 and r.rankPoints == 0)
        r = planMissionRank(5, 6, 100, 50)
        assert(r.rankPoints == 0 and r.setRank and r.newRank == 6)
        r = planMissionRank(6, 6, 100, nil)
        assert(not r.setRank and not r.applyRP and r.rankPoints == 100)
        -- RP only (no rank up): clamp to cap
        r = planMissionRank(6, 6, 3950, 100)
        assert(r.applyRP and not r.setRank and r.rankPoints == 4000)
        r = planMissionRank(3, 5, 3990, 20)
        assert(r.setRank and r.rankPoints == 0)
    end)
end)
