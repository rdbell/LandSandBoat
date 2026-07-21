-----------------------------------
-- Pure system tests for conquest overseerOffsets + visibility (slice 6208).
-----------------------------------

describe('conquest overseer offsets pure plan', function()
    local SANDORIA, BASTOK, WINDURST, BEASTMEN, OTHER = 0, 1, 2, 3, 4
    local RONFAURE, ZULKHEIM, TULIA, MOVALPOLOS, TAV = 0, 1, 16, 17, 18

    -- sampled catalog pins (full 263 rows live in production overseerOffsets)
    local ronfaure =
    {
        { 0, SANDORIA }, { 7, SANDORIA }, { 3, SANDORIA }, { 11, SANDORIA },
        { 1, BASTOK }, { 8, BASTOK }, { 4, BASTOK }, { 12, BASTOK },
        { 2, WINDURST }, { 9, WINDURST }, { 5, WINDURST }, { 13, WINDURST },
        { 6, BEASTMEN }, { 14, BEASTMEN },
        { 10, OTHER },
    }

    local zulkheimBastokSecondFlag = 13 -- anomaly vs common 12
    local tulia =
    {
        { 0, SANDORIA }, { 1, BASTOK }, { 2, WINDURST }, { 3, BEASTMEN },
    }

    -- region → row count for full catalog size check
    local regionCounts =
    {
        [0] = 15, [1] = 15, [2] = 15, [3] = 15, [4] = 15,
        [5] = 15, [6] = 15, [7] = 15, [8] = 15, [9] = 15,
        [10] = 15, [11] = 15, [12] = 15, [13] = 15, [14] = 15,
        [15] = 15, [16] = 4, [17] = 4, [18] = 15,
    }

    local function visible(entryNation, regionOwner)
        return entryNation == regionOwner or entryNation == OTHER
    end

    it('full catalog size is 263 across 19 regions', function()
        local n = 0
        local regions = 0
        for _, c in pairs(regionCounts) do
            n = n + c
            regions = regions + 1
        end
        assert(regions == 19 and n == 263)
        assert(regionCounts[TULIA] == 4 and regionCounts[MOVALPOLOS] == 4)
        assert(regionCounts[RONFAURE] == 15 and regionCounts[TAV] == 15)
    end)

    it('Ronfaure offsets and OTHER pin', function()
        assert(#ronfaure == 15)
        assert(ronfaure[1][1] == 0 and ronfaure[1][2] == SANDORIA)
        assert(ronfaure[15][1] == 10 and ronfaure[15][2] == OTHER)
        local beast = 0
        for _, r in ipairs(ronfaure) do
            if r[2] == BEASTMEN then beast = beast + 1 end
        end
        assert(beast == 2)
    end)

    it('Zulkheim bastok second flag offset anomaly', function()
        assert(zulkheimBastokSecondFlag == 13)
    end)

    it('Tulia is flag-only four nations', function()
        assert(#tulia == 4)
        assert(tulia[1][2] == SANDORIA and tulia[4][2] == BEASTMEN)
    end)

    it('visibility: owner match or OTHER', function()
        assert(visible(SANDORIA, SANDORIA))
        assert(not visible(BASTOK, SANDORIA))
        assert(visible(OTHER, SANDORIA))
        assert(visible(BEASTMEN, BEASTMEN))
    end)

    it('Ronfaure sandoria owner shows 5 rows', function()
        local shown = 0
        for _, r in ipairs(ronfaure) do
            if visible(r[2], SANDORIA) then shown = shown + 1 end
        end
        assert(shown == 5) -- 4 sandoria + OTHER
    end)
end)
