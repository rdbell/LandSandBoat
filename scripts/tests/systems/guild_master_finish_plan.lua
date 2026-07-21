-----------------------------------
-- Pure system tests for guild master finish / renounce (slice 6152).
-----------------------------------

describe('guild master finish pure plan', function()
    local WOODWORKING = 49
    local COOKING     = 56
    local FISHING     = 48

    local ACT_NONE       = 0
    local ACT_SIGNUP     = 1
    local ACT_SIGNUP_FULL = 2
    local ACT_EXPERT_START = 3
    local ACT_EXPERT_READY = 4
    local ACT_RENOUNCE   = 5
    local ACT_TRADE_COMPLETE = 6

    local function rankFromSetting(craftCommonCap)
        return math.floor(craftCommonCap / 100)
    end

    local function renounceRank(craftCommonCap)
        return rankFromSetting(craftCommonCap) - 1
    end

    local function planFinish(p)
        if p.csid == p.eventId then
            if p.option == 1 then
                if p.freeSlots == 0 then
                    return { action = ACT_SIGNUP_FULL, crystalId = p.crystalId }
                end
                return { action = ACT_SIGNUP, crystalId = p.crystalId, guildBit = p.guildId }
            end
            if p.option == 2 then
                if p.joined and p.expertVar == 0 then
                    return { action = ACT_EXPERT_START, setExpertVar = 1 }
                end
                return { action = ACT_NONE }
            end
            if p.option == 3 then
                return { action = ACT_EXPERT_READY, setExpertVar = 2 }
            end
            if p.option >= WOODWORKING and p.option <= COOKING then
                local r = renounceRank(p.craftCommonCap)
                return {
                    action = ACT_RENOUNCE,
                    skillId = p.option,
                    newRank = r,
                    newLevel = p.craftCommonCap,
                    msgSkillOffset = p.option - 49,
                }
            end
            return { action = ACT_NONE }
        end
        if p.csid == p.eventId + 1 and p.completeTrade == 1 then
            return { action = ACT_TRADE_COMPLETE }
        end
        return { action = ACT_NONE }
    end

    -- artisan bitmask: bit set when rank < threshold OR skill is highest
    local function artisanParams(ranks, levels, rankThreshold)
        -- ranks/levels keyed by skill 49..56
        local highestSkill, highestLevel = 0, -1
        for skill = WOODWORKING, COOKING do
            local lvl = levels[skill] or 0
            if lvl > highestLevel then
                highestLevel = lvl
                highestSkill = skill
            end
        end
        local count, mask = 0, 0
        for skill = WOODWORKING, COOKING do
            local rank = ranks[skill] or 0
            if rank >= rankThreshold then
                count = count + 1
            end
            if rank < rankThreshold or skill == highestSkill then
                mask = bit.bor(mask, bit.lshift(1, skill - 48))
            end
        end
        return count, mask, highestSkill
    end

    local function roeRecord(guildId)
        if guildId == 0 then return nil end -- fishing
        return guildId + 99
    end

    it('signup free slots and expert options', function()
        local r = planFinish({
            csid = 100, eventId = 100, option = 1, freeSlots = 0, crystalId = 4096, guildId = 1,
            joined = true, expertVar = 0, craftCommonCap = 700, completeTrade = 0,
        })
        assert(r.action == ACT_SIGNUP_FULL)
        r = planFinish({
            csid = 100, eventId = 100, option = 1, freeSlots = 2, crystalId = 4096, guildId = 1,
            joined = true, expertVar = 0, craftCommonCap = 700, completeTrade = 0,
        })
        assert(r.action == ACT_SIGNUP and r.guildBit == 1)
        r = planFinish({
            csid = 100, eventId = 100, option = 2, freeSlots = 1, crystalId = 0, guildId = 1,
            joined = true, expertVar = 0, craftCommonCap = 700, completeTrade = 0,
        })
        assert(r.action == ACT_EXPERT_START and r.setExpertVar == 1)
        r = planFinish({
            csid = 100, eventId = 100, option = 2, freeSlots = 1, crystalId = 0, guildId = 1,
            joined = false, expertVar = 0, craftCommonCap = 700, completeTrade = 0,
        })
        assert(r.action == ACT_NONE)
        r = planFinish({
            csid = 100, eventId = 100, option = 3, freeSlots = 1, crystalId = 0, guildId = 1,
            joined = true, expertVar = 1, craftCommonCap = 700, completeTrade = 0,
        })
        assert(r.action == ACT_EXPERT_READY and r.setExpertVar == 2)
    end)

    it('renounce rank and trade complete', function()
        assert(rankFromSetting(700) == 7 and renounceRank(700) == 6)
        local r = planFinish({
            csid = 100, eventId = 100, option = WOODWORKING + 1, -- smithing 50
            freeSlots = 1, crystalId = 0, guildId = 2,
            joined = true, expertVar = 0, craftCommonCap = 700, completeTrade = 0,
        })
        assert(r.action == ACT_RENOUNCE and r.newRank == 6 and r.newLevel == 700)
        assert(r.msgSkillOffset == 1) -- 50 - 49
        r = planFinish({
            csid = 101, eventId = 100, option = 0, freeSlots = 0, crystalId = 0, guildId = 1,
            joined = true, expertVar = 0, craftCommonCap = 700, completeTrade = 1,
        })
        assert(r.action == ACT_TRADE_COMPLETE)
    end)

    it('artisan renounce params', function()
        local ranks = {
            [49] = 7, [50] = 6, [51] = 8, [52] = 3, [53] = 3, [54] = 3, [55] = 3, [56] = 3,
        }
        local levels = {
            [49] = 700, [50] = 600, [51] = 800, [52] = 100, [53] = 100, [54] = 100, [55] = 100, [56] = 100,
        }
        local count, mask, highest = artisanParams(ranks, levels, 7)
        assert(highest == 51) -- goldsmithing highest level
        assert(count == 2) -- wood 7 + gold 8
        -- bits for skills that cannot renounce: rank < 7 OR highest
        -- wood 49: rank 7 >= 7 but not highest → not in mask
        -- gold 51: highest → in mask
        -- smith 50: rank 6 < 7 → in mask
        assert(bit.band(mask, bit.lshift(1, 51 - 48)) ~= 0)
        assert(bit.band(mask, bit.lshift(1, 50 - 48)) ~= 0)
        assert(bit.band(mask, bit.lshift(1, 49 - 48)) == 0)
    end)

    it('roe record skips fishing', function()
        assert(roeRecord(0) == nil)
        assert(roeRecord(1) == 100)
        assert(roeRecord(2) == 101)
    end)
end)
