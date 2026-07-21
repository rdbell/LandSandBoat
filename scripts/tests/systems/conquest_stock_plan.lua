-----------------------------------
-- Pure system tests for conquest overseerInvCommon + getStock pure (slice 6202).
-----------------------------------

describe('conquest overseer stock pure plan', function()
    local OTHER = 4
    local SANDORIA, BASTOK, WINDURST = 0, 1, 2

    -- overseerInvCommon (option → {cp, lvl, item, rank?})
    local common =
    {
        [32928] = { cp = 7,     lvl = 1, item = 4182 },  -- SCROLL_OF_INSTANT_RERAISE
        [32929] = { cp = 10,    lvl = 1, item = 4181 },  -- SCROLL_OF_INSTANT_WARP
        [32930] = { cp = 2500,  lvl = 1, item = 15542 }, -- RETURN_RING
        [32931] = { cp = 9000,  lvl = 1, item = 15541 }, -- HOMING_RING
        [32933] = { cp = 500,   lvl = 1, item = 15761 }, -- CHARIOT_BAND
        [32934] = { cp = 1000,  lvl = 1, item = 15762 }, -- EMPRESS_BAND
        [32935] = { cp = 2000,  lvl = 1, item = 15763 }, -- EMPEROR_BAND
        [32936] = { cp = 5000,  lvl = 1, item = 28540 }, -- WARP_RING
        [32937] = { cp = 1000,  lvl = 1, item = 10114 }, -- CIPHER TENZEN
        [32938] = { cp = 1000,  lvl = 1, item = 10139 }, -- CIPHER RAHAL
        [32939] = { cp = 1000,  lvl = 1, item = 10146 }, -- CIPHER KUKKI
        [32941] = { cp = 20000, lvl = 1, item = 6380, rank = 10 }, -- REFINED_CHAIR_SET
        [32942] = { cp = 1000,  lvl = 1, item = 10180 }, -- CIPHER MAKKI
    }

    -- minimal nation sample for getStock routing (one real first-row item each)
    local nation =
    {
        [SANDORIA] = { [32768] = { rank = 1, cp = 1000, lvl = 10, item = 17167 } }, -- ROYAL_ARCHERS_LONGBOW
        [BASTOK]   = { [32768] = { rank = 1, cp = 1000, lvl = 10, item = 16433 } }, -- LEGIONNAIRES_KNUCKLES
        [WINDURST] = { [32768] = { rank = 1, cp = 1000, lvl = 10, item = 17159 } }, -- FREESWORDS_BOW
    }

    local function getStock(playerNation, guardNation, option)
        local r = common[option]
        if r == nil then
            if guardNation == OTHER then
                local n = nation[playerNation]
                r = n and n[option]
            else
                local n = nation[guardNation]
                r = n and n[option]
            end
        end
        return r
    end

    it('common catalog size and pins', function()
        local n = 0
        for _ in pairs(common) do n = n + 1 end
        assert(n == 13)
        assert(common[32928].item == 4182 and common[32928].cp == 7)
        assert(common[32941].rank == 10 and common[32941].cp == 20000)
        assert(common[32932] == nil) -- gap between 32931 and 32933
        assert(common[32940] == nil) -- gap before 32941
    end)

    it('getStock prefers common over nation', function()
        local r = getStock(SANDORIA, SANDORIA, 32928)
        assert(r and r.item == 4182)
        -- nation option only when not in common
        r = getStock(SANDORIA, SANDORIA, 32768)
        assert(r and r.item == 17167)
        -- OTHER uses player nation
        r = getStock(BASTOK, OTHER, 32768)
        assert(r and r.item == 16433)
        r = getStock(WINDURST, OTHER, 32768)
        assert(r and r.item == 17159)
        -- missing
        assert(getStock(SANDORIA, SANDORIA, 99999) == nil)
    end)

    it('exp ring options are common stock', function()
        assert(common[32933].item == 15761)
        assert(common[32934].item == 15762)
        assert(common[32935].item == 15763)
    end)
end)
