-----------------------------------
-- Pure system tests for beastmen treasure event pure plans (slice 6190).
-----------------------------------

describe('beastmen treasure event pure plan', function()
    local QUEST_AVAILABLE  = 0
    local QUEST_ACCEPTED   = 1
    local QUEST_COMPLETED  = 2

    -- Peddlestox onTrigger branch
    local BRANCH_NO_MAP   = 102
    local BRANCH_FETCH    = 100
    local BRANCH_CHARITY  = 104
    local BRANCH_REMIND   = 103
    local BRANCH_NONE     = 0

    local function planNpcTrigger(hasMap, status)
        if not hasMap then
            return BRANCH_NO_MAP
        end
        if status == QUEST_AVAILABLE then
            return BRANCH_FETCH
        end
        if status == QUEST_ACCEPTED then
            return BRANCH_CHARITY
        end
        if status == QUEST_COMPLETED then
            return BRANCH_REMIND
        end
        return BRANCH_NONE
    end

    local function planNpcTrade(status, tradeExact)
        return status == QUEST_ACCEPTED and tradeExact
    end

    local function planNpcFinish(csid)
        if csid == 100 then
            return 'accept'
        end
        if csid == 101 then
            return 'complete'
        end
        return 'none'
    end

    -- Peddlestox day: show when zone day matches; hide when currently normal and not match
    local PED_SHOW = 1
    local PED_HIDE = 2
    local PED_IDLE = 0

    local function planPeddlestox(zoneDay, vanaDay, currentlyNormal)
        if zoneDay == vanaDay then
            return PED_SHOW
        end
        if currentlyNormal then
            return PED_HIDE
        end
        return PED_IDLE
    end

    local function planQmTrigger(assignedID, npcID)
        if assignedID == nil or assignedID ~= npcID then
            return 'nothing'
        end
        return 'buried'
    end

    local function planQmTrade(hasPickaxe, status, npcID, digsiteID)
        return hasPickaxe and status == QUEST_COMPLETED and npcID == digsiteID
    end

    local function planExcavateReset()
        return QUEST_AVAILABLE, 0 -- status, digsite var
    end

    it('npc trigger branches', function()
        assert(planNpcTrigger(false, QUEST_AVAILABLE) == BRANCH_NO_MAP)
        assert(planNpcTrigger(true, QUEST_AVAILABLE) == BRANCH_FETCH)
        assert(planNpcTrigger(true, QUEST_ACCEPTED) == BRANCH_CHARITY)
        assert(planNpcTrigger(true, QUEST_COMPLETED) == BRANCH_REMIND)
    end)

    it('npc trade and finish', function()
        assert(planNpcTrade(QUEST_ACCEPTED, true))
        assert(not planNpcTrade(QUEST_AVAILABLE, true))
        assert(not planNpcTrade(QUEST_ACCEPTED, false))
        assert(planNpcFinish(100) == 'accept')
        assert(planNpcFinish(101) == 'complete')
        assert(planNpcFinish(99) == 'none')
    end)

    it('peddlestox day visibility', function()
        assert(planPeddlestox(3, 3, false) == PED_SHOW)
        assert(planPeddlestox(3, 1, true) == PED_HIDE)
        assert(planPeddlestox(3, 1, false) == PED_IDLE)
    end)

    it('qm trigger and trade', function()
        assert(planQmTrigger(nil, 100) == 'nothing')
        assert(planQmTrigger(100, 101) == 'nothing')
        assert(planQmTrigger(100, 100) == 'buried')
        assert(planQmTrade(true, QUEST_COMPLETED, 5, 5))
        assert(not planQmTrade(false, QUEST_COMPLETED, 5, 5))
        assert(not planQmTrade(true, QUEST_ACCEPTED, 5, 5))
        local st, ds = planExcavateReset()
        assert(st == QUEST_AVAILABLE and ds == 0)
    end)
end)
