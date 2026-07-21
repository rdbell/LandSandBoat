-----------------------------------
-- Pure system tests for fishing contest event plans (slice 6154).
-----------------------------------

describe('fishing contest event pure plan', function()
    local ACCEPTING  = 2
    local PRESENTING = 4
    local CLOSED     = 6

    local function isRewardAvailable(rank)
        if rank and rank > 0 and rank <= 20 then
            return 1
        end
        return 0
    end

    local function playerReward(rank, share, baseGil)
        if not rank or rank <= 0 or rank > 20 then
            return nil
        end
        share = share or 1
        local gil = baseGil
        if gil then
            gil = gil / share
        end
        return { rank = rank, gil = gil }
    end

    -- Trade submit gates for onTrade
    local TRADE_NONE     = 0
    local TRADE_ALREADY  = 1 -- startEvent 10007 param4=1
    local TRADE_VALID    = 2 -- startEvent with score

    local function planTrade(status, hasExactFish, isRanked, size, weight)
        if status ~= ACCEPTING or not hasExactFish then
            return { path = TRADE_NONE }
        end
        if isRanked or size == 0 or weight == 0 then
            return { path = TRADE_ALREADY }
        end
        return { path = TRADE_VALID, size = size, weight = weight }
    end

    local function canPayFee(gil)
        return gil >= 500
    end

    -- Finish submit: option 145 and fee paid
    local function planFinishSubmit(csid, option, gil, length, weight)
        if csid ~= 10007 or option ~= 145 then
            return { submit = false }
        end
        if not canPayFee(gil) then
            return { submit = false, needGil = true }
        end
        if length == 0 or weight == 0 then
            return { submit = false }
        end
        return { submit = true, length = length, weight = weight, fee = 500 }
    end

    it('reward available flag', function()
        assert(isRewardAvailable(1) == 1 and isRewardAvailable(20) == 1)
        assert(isRewardAvailable(0) == 0 and isRewardAvailable(21) == 0)
        assert(isRewardAvailable(nil) == 0)
    end)

    it('player reward gil shared', function()
        local r = playerReward(1, 2, 10000)
        assert(r.gil == 5000)
        assert(playerReward(0, 1, 100) == nil)
        assert(playerReward(21, 1, 100) == nil)
    end)

    it('trade submit paths', function()
        assert(planTrade(CLOSED, true, false, 10, 20).path == TRADE_NONE)
        assert(planTrade(ACCEPTING, false, false, 10, 20).path == TRADE_NONE)
        assert(planTrade(ACCEPTING, true, true, 10, 20).path == TRADE_ALREADY)
        assert(planTrade(ACCEPTING, true, false, 0, 20).path == TRADE_ALREADY)
        local r = planTrade(ACCEPTING, true, false, 15, 30)
        assert(r.path == TRADE_VALID and r.size == 15 and r.weight == 30)
    end)

    it('registration fee and finish', function()
        assert(canPayFee(500) and not canPayFee(499))
        local r = planFinishSubmit(10007, 145, 500, 10, 20)
        assert(r.submit and r.fee == 500)
        assert(planFinishSubmit(10007, 145, 499, 10, 20).needGil == true)
        assert(planFinishSubmit(10007, 144, 500, 10, 20).submit == false)
        assert(planFinishSubmit(10006, 145, 500, 10, 20).submit == false)
    end)
end)
