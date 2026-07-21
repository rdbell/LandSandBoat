-----------------------------------
-- Pure system tests for image support event plans (slice 6151).
-----------------------------------

describe('image support event pure plan', function()
    local function gilCost(tier, skillRank)
        if tier == 0 then
            return (skillRank + 1) * 30
        end
        return 0
    end

    local function canAfford(gil, cost)
        return gil >= cost
    end

    -- Old finish: csid==eventId and option==1 and gil >= cost
    local function planOldFinish(csid, eventId, option, gil, tier, skillRank)
        local cost = gilCost(tier, skillRank)
        if csid ~= eventId or option ~= 1 or not canAfford(gil, cost) then
            return { apply = false, cost = cost }
        end
        local advanced = cost > 0
        return { apply = true, cost = cost, advanced = advanced, free = not advanced }
    end

    -- Aht trade: joined guild, single bronze piece, no active effect
    local function planAhtTrade(joined, hasBronze, itemCount, hasEffect)
        if not joined then
            return { start = false }
        end
        if not (hasBronze and itemCount == 1) then
            return { start = false }
        end
        if hasEffect then
            return { start = false, active = true }
        end
        return { start = true }
    end

    -- Aht finish: free on option 1 same csid; advanced on trade csid (+1)
    local function planAhtFinish(csid, triggerCSID, option)
        if csid == triggerCSID and option == 1 then
            return { apply = true, advanced = false }
        end
        if csid == triggerCSID + 1 then
            return { apply = true, advanced = true }
        end
        return { apply = false }
    end

    -- Gold/Alchemy first-speak bit set
    local function firstSpeakBit(guildId)
        if guildId == 3 then return 24 end -- goldsmithing
        if guildId == 7 then return 25 end -- alchemy
        return nil
    end

    local function needsFirstSpeak(guildId, joinedMask, spokenBitSet)
        local bitn = firstSpeakBit(guildId)
        if not bitn then return false end
        local joined = bit.band(joinedMask, bit.lshift(1, guildId)) ~= 0
        return joined and not spokenBitSet
    end

    it('old finish gil cost and apply gates', function()
        local r = planOldFinish(100, 100, 1, 90, 0, 2) -- cost 90
        assert(r.apply and r.advanced and r.cost == 90)
        r = planOldFinish(100, 100, 1, 89, 0, 2)
        assert(not r.apply and r.cost == 90)
        r = planOldFinish(100, 100, 1, 0, 1, 5) -- free tier
        assert(r.apply and r.free and r.cost == 0)
        r = planOldFinish(100, 100, 0, 999, 0, 2)
        assert(not r.apply)
        r = planOldFinish(99, 100, 1, 999, 0, 2)
        assert(not r.apply)
    end)

    it('aht trade and finish paths', function()
        assert(planAhtTrade(true, true, 1, false).start == true)
        assert(planAhtTrade(false, true, 1, false).start == false)
        assert(planAhtTrade(true, true, 2, false).start == false)
        local r = planAhtTrade(true, true, 1, true)
        assert(not r.start and r.active)
        assert(planAhtFinish(50, 50, 1).advanced == false and planAhtFinish(50, 50, 1).apply)
        assert(planAhtFinish(51, 50, 0).advanced == true)
        assert(planAhtFinish(50, 50, 0).apply == false)
    end)

    it('first-speak guild bits', function()
        assert(firstSpeakBit(3) == 24 and firstSpeakBit(7) == 25)
        assert(firstSpeakBit(1) == nil)
        -- joined goldsmithing (bit 3), not spoken (bit 24)
        local mask = bit.lshift(1, 3)
        assert(needsFirstSpeak(3, mask, false) == true)
        assert(needsFirstSpeak(3, mask, true) == false)
        assert(needsFirstSpeak(3, 0, false) == false)
    end)
end)
