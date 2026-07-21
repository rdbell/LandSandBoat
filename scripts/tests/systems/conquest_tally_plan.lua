-----------------------------------
-- Pure system tests for conquest tally message pure plans (slice 6200).
-----------------------------------

describe('conquest tally message pure plan', function()
    -- owner control message offset for tally end
    local function planOwnerEndOffset(owner)
        if owner <= 3 then
            return 2 + owner -- base+2..+5 nations
        end
        return 6 -- beastmen
    end

    -- update message owner offset
    local function planOwnerUpdateOffset(owner)
        if owner == 255 then
            return nil -- skip regional influence
        end
        if owner <= 3 then
            return 32 + owner
        end
        return 31 -- beastmen
    end

    -- influence nation tiers from bitfields
    local function planInfluenceTiers(influence)
        if influence >= 64 then
            return 'beastmen'
        end
        if influence == 0 then
            return 'deadlock'
        end
        local sandoria = bit.band(influence, 0x03)
        local bastok   = bit.rshift(bit.band(influence, 0x0C), 2)
        local windurst = bit.rshift(bit.band(influence, 0x30), 4)
        return sandoria, bastok, windurst
    end

    local function planInfluenceMsgOffsets(sandoria, bastok, windurst)
        return 41 - sandoria, 45 - bastok, 49 - windurst
    end

    -- alliance message offsets for balance of power (tally end)
    local function planAllianceEndMsg(ranking)
        if bit.band(ranking, 0x03) == 0x01 then
            return 50 -- Bastok+Windurst
        end
        if bit.band(ranking, 0x0C) == 0x04 then
            return 51 -- San d'Oria+Windurst
        end
        if bit.band(ranking, 0x30) == 0x10 then
            return 52 -- San d'Oria+Bastok
        end
        return nil
    end

    -- update alliance messages
    local function planAllianceUpdateMsg(ranking)
        if bit.band(ranking, 0x03) == 0x01 then
            return 53
        end
        if bit.band(ranking, 0x0C) == 0x04 then
            return 54
        end
        if bit.band(ranking, 0x30) == 0x10 then
            return 55
        end
        return nil
    end

    -- balance of power offset (simplified samples from known ranking patterns)
    local function planBalanceOffset(ranking)
        local offset = 0
        if bit.band(ranking, 0x03) == 0x01 then
            offset = offset + 7
            if bit.band(ranking, 0x30) == 0x10 then
                offset = offset + 1
                if bit.band(ranking, 0x0C) == 0x0C then
                    offset = offset + 1
                end
            elseif bit.band(ranking, 0x0C) == 0x08 then
                offset = offset + 3
                if bit.band(ranking, 0x30) == 0x30 then
                    offset = offset + 1
                end
            elseif bit.band(ranking, 0x0C) == 0x04 then
                offset = offset + 6
            end
        elseif bit.band(ranking, 0x0C) == 0x04 then
            offset = offset + 15
            if bit.band(ranking, 0x30) == 0x02 then
                offset = offset + 3
                if bit.band(ranking, 0x03) == 0x03 then
                    offset = offset + 1
                end
            elseif bit.band(ranking, 0x30) == 0x10 then
                offset = offset + 6
            end
        elseif bit.band(ranking, 0x30) == 0x10 then
            offset = offset + 23
            if bit.band(ranking, 0x0C) == 0x08 then
                offset = offset + 3
                if bit.band(ranking, 0x30) == 0x30 then
                    offset = offset + 1
                end
            end
        end
        return offset
    end

    -- city region update routing (TAVNAZIANARCH=18, DYNAMIS=23)
    local TAVNAZIAN = 18
    local DYNAMIS = 23
    local function planIsCityRegion(regionId)
        return regionId > TAVNAZIAN and regionId < DYNAMIS
    end

    it('owner message offsets', function()
        assert(planOwnerEndOffset(0) == 2)
        assert(planOwnerEndOffset(3) == 5)
        assert(planOwnerEndOffset(4) == 6)
        assert(planOwnerUpdateOffset(255) == nil)
        assert(planOwnerUpdateOffset(0) == 32)
        assert(planOwnerUpdateOffset(3) == 35)
        assert(planOwnerUpdateOffset(4) == 31)
    end)

    it('influence tiers and msg offsets', function()
        assert(planInfluenceTiers(64) == 'beastmen')
        assert(planInfluenceTiers(0) == 'deadlock')
        local s, b, w = planInfluenceTiers(0x15) -- 010101: s=1,b=1,w=1
        assert(s == 1 and b == 1 and w == 1)
        local os, ob, ow = planInfluenceMsgOffsets(1, 1, 1)
        assert(os == 40 and ob == 44 and ow == 48)
    end)

    it('alliance messages', function()
        assert(planAllianceEndMsg(0x01) == 50)
        assert(planAllianceEndMsg(0x04) == 51)
        assert(planAllianceEndMsg(0x10) == 52)
        assert(planAllianceEndMsg(0) == nil)
        assert(planAllianceUpdateMsg(0x01) == 53)
        assert(planAllianceUpdateMsg(0x04) == 54)
        assert(planAllianceUpdateMsg(0x10) == 55)
    end)

    it('balance offset samples', function()
        assert(planBalanceOffset(0x01) == 7)
        assert(planBalanceOffset(0x01 + 0x10) == 8) -- 0x11
        assert(planBalanceOffset(0x04) == 15)
        assert(planBalanceOffset(0x10) == 23)
        assert(planBalanceOffset(0) == 0)
    end)

    it('city region gate', function()
        assert(not planIsCityRegion(18))
        assert(planIsCityRegion(19))
        assert(planIsCityRegion(22))
        assert(not planIsCityRegion(23))
    end)
end)
