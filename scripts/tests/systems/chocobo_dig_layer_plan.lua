-----------------------------------
-- Pure system tests for chocobo dig handleDiggingLayer (slice 6145).
-----------------------------------

describe('chocobo dig layer pure plan', function()
    -- Craft ranks (xi.craftRank)
    local NOVICE    = 3
    local CRAFTSMAN = 6

    local function entryPasses(roll, digRate, playerRank, craftRank)
        return roll <= digRate and playerRank >= craftRank
    end

    local function weatherCrystalPasses(roll)
        return roll <= 100
    end

    local function geodePasses(playerRank, roll)
        return playerRank >= NOVICE and roll <= 50
    end

    local function oreMoonOK(moon)
        return moon >= 7 and moon <= 21
    end

    local function oreEnvOK(isOreZone, weatherElementIsNone, moon)
        return isOreZone and not weatherElementIsNone and oreMoonOK(moon)
    end

    local function orePasses(envOK, playerRank, roll)
        return envOK and playerRank >= CRAFTSMAN and roll <= 100
    end

    -- entries: { { itemId, weight, craftRank, roll }, ... }
    -- bonus: { enabled, weatherId, weatherRoll, geodeId, geodeRoll, oreId, oreEnvOK, oreRoll }
    local function planCandidates(entries, playerRank, hasRare, bonus)
        local candidates = {}
        for _, e in ipairs(entries) do
            local digRate = e[2]
            if hasRare then
                if digRate >= 100 then
                    digRate = math.floor(digRate / 2)
                else
                    digRate = digRate * 2
                end
            end
            if entryPasses(e[4], digRate, playerRank, e[3]) then
                candidates[#candidates + 1] = e[1]
            end
        end
        if bonus and bonus.enabled then
            if bonus.weatherId and bonus.weatherId > 0 and weatherCrystalPasses(bonus.weatherRoll) then
                candidates[#candidates + 1] = bonus.weatherId
            end
            if bonus.geodeId and bonus.geodeId > 0 and geodePasses(playerRank, bonus.geodeRoll) then
                candidates[#candidates + 1] = bonus.geodeId
            end
            if bonus.oreId and bonus.oreId > 0 and orePasses(bonus.oreEnvOK, playerRank, bonus.oreRoll) then
                candidates[#candidates + 1] = bonus.oreId
            end
        end
        return candidates
    end

    local function pick(candidates, pickIdx)
        if #candidates == 0 or pickIdx < 1 or pickIdx > #candidates then
            return 0
        end
        return candidates[pickIdx]
    end

    local function planReward(entries, playerRank, hasRare, bonus, pickIdx)
        return pick(planCandidates(entries, playerRank, hasRare, bonus), pickIdx)
    end

    it('entry gate needs roll and rank', function()
        assert(entryPasses(50, 100, 0, 0) == true)
        assert(entryPasses(101, 100, 0, 0) == false)
        assert(entryPasses(50, 100, 2, 3) == false)
        assert(entryPasses(50, 100, 3, 3) == true)
    end)

    it('empty table yields no reward', function()
        assert(planReward({}, 0, false, nil, 1) == 0)
    end)

    it('collects passing dig table rows', function()
        local entries = {
            { 10, 100, 0, 50 },  -- pass
            { 20, 100, 5, 50 },  -- rank fail
            { 30, 10,  0, 50 },  -- roll fail
            { 40, 200, 0, 100 }, -- pass
        }
        local c = planCandidates(entries, 0, false, nil)
        assert(#c == 2 and c[1] == 10 and c[2] == 40)
        assert(planReward(entries, 0, false, nil, 2) == 40)
    end)

    it('applies rare ability rate adjust', function()
        -- weight 50 * 2 = 100 with rare; roll 80 passes
        local entries = { { 99, 50, 0, 80 } }
        assert(#planCandidates(entries, 0, false, nil) == 0)
        assert(#planCandidates(entries, 0, true, nil) == 1)
        -- weight 200 / 2 = 100 with rare; roll 100 passes, 101 fails
        local hi = { { 88, 200, 0, 100 } }
        assert(#planCandidates(hi, 0, true, nil) == 1)
        hi[1][4] = 101
        assert(#planCandidates(hi, 0, true, nil) == 0)
    end)

    it('regular layer weather geode ore bonuses', function()
        local bonus = {
            enabled     = true,
            weatherId   = 111,
            weatherRoll = 100, -- pass
            geodeId     = 222,
            geodeRoll   = 50,  -- needs NOVICE+
            oreId       = 333,
            oreEnvOK    = true,
            oreRoll     = 100, -- needs CRAFTSMAN+
        }
        -- rank 0: only weather
        local c = planCandidates({}, 0, false, bonus)
        assert(#c == 1 and c[1] == 111)
        -- rank NOVICE: weather + geode
        c = planCandidates({}, NOVICE, false, bonus)
        assert(#c == 2 and c[1] == 111 and c[2] == 222)
        -- rank CRAFTSMAN: all three
        c = planCandidates({}, CRAFTSMAN, false, bonus)
        assert(#c == 3 and c[3] == 333)
        -- weather roll fail
        bonus.weatherRoll = 101
        c = planCandidates({}, CRAFTSMAN, false, bonus)
        assert(#c == 2 and c[1] == 222 and c[2] == 333)
    end)

    it('elemental ore environment and moon gates', function()
        assert(oreMoonOK(7) and oreMoonOK(21) and not oreMoonOK(6) and not oreMoonOK(22))
        assert(oreEnvOK(true, false, 10) == true)
        assert(oreEnvOK(false, false, 10) == false)
        assert(oreEnvOK(true, true, 10) == false)
        assert(oreEnvOK(true, false, 6) == false)
        assert(orePasses(true, CRAFTSMAN, 100) == true)
        assert(orePasses(true, CRAFTSMAN - 1, 100) == false)
        assert(orePasses(false, CRAFTSMAN, 50) == false)
    end)

    it('non-regular layer skips bonuses', function()
        local bonus = {
            enabled     = false,
            weatherId   = 111,
            weatherRoll = 1,
            geodeId     = 222,
            geodeRoll   = 1,
            oreId       = 333,
            oreEnvOK    = true,
            oreRoll     = 1,
        }
        assert(#planCandidates({}, CRAFTSMAN, false, bonus) == 0)
    end)
end)
