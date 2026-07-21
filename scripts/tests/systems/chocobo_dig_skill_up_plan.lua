-----------------------------------
-- Pure system tests for chocobo dig calculateSkillUp (slice 6144).
-----------------------------------

describe('chocobo dig skill-up pure plan', function()
    local function digSkillCap(rank)
        local v = (rank + 1) * 100
        if v < 0 then return 0 end
        if v > 1000 then return 1000 end
        return v
    end

    local function increment(real, maxSkill, inc)
        if real >= maxSkill or inc <= 0 then return 0 end
        if real + inc > maxSkill then return maxSkill - real end
        return inc
    end

    local function rollOK(roll)
        return roll >= 1 and roll <= 15
    end

    local function rankUp(real, inc, rank)
        return real + inc >= rank * 100 + 100
    end

    local function plan(rank, real, roll)
        local maxSkill = digSkillCap(rank)
        local inc = increment(real, maxSkill, 1)
        if inc <= 0 or not rollOK(roll) then
            return { skillUp = false }
        end
        local r = { skillUp = true, newSkill = real + inc, increment = inc }
        if rankUp(real, inc, rank) then
            r.rankUp = true
            r.newRank = rank + 1
        end
        return r
    end

    it('blocks when capped or roll fails', function()
        assert(plan(0, 100, 1).skillUp == false)
        assert(plan(0, 50, 16).skillUp == false)
    end)

    it('skills up without rank at mid band', function()
        local r = plan(0, 50, 10)
        assert(r.skillUp == true and r.newSkill == 51 and not r.rankUp)
    end)

    it('ranks up at 10.0 boundary', function()
        local r = plan(0, 99, 1)
        assert(r.skillUp == true and r.rankUp == true and r.newRank == 1)
    end)
end)
