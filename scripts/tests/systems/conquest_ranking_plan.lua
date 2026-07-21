-----------------------------------
-- Pure system tests for conquest ranking + regional NPC visibility (slice 6206).
-----------------------------------

describe('conquest ranking pure plan', function()
    local SOUTHERN_SAN_DORIA = 230
    local PORT_BASTOK = 236
    local WINDURST_WOODS = 241

    local function conquestRanking(sandoria, bastok, windurst)
        return sandoria + 4 * bastok + 16 * windurst
    end

    local function isRegionalNPCZone(zoneID)
        return zoneID == SOUTHERN_SAN_DORIA or zoneID == PORT_BASTOK or zoneID == WINDURST_WOODS
    end

    -- toggleRegionalNPCs pure: sort ranks ascending, hide on first-place tie,
    -- else show only in the sole first-place city.
    local function showRegionalNPCs(zoneID, sd, bastok, windy)
        if not isRegionalNPCZone(zoneID) then
            return false
        end
        local rankings =
        {
            { sd, SOUTHERN_SAN_DORIA },
            { bastok, PORT_BASTOK },
            { windy, WINDURST_WOODS },
        }
        table.sort(rankings, function(a, b)
            return a[1] < b[1]
        end)
        local firstAndSecondTie = rankings[1][1] == rankings[2][1]
        if firstAndSecondTie then
            return false
        end
        return zoneID == rankings[1][2]
    end

    it('conquestRanking packs three nation ranks', function()
        assert(conquestRanking(1, 2, 3) == 1 + 8 + 48)
        assert(conquestRanking(1, 1, 1) == 1 + 4 + 16)
        assert(conquestRanking(3, 2, 1) == 3 + 8 + 16)
    end)

    it('regional NPC city zone pins', function()
        assert(isRegionalNPCZone(SOUTHERN_SAN_DORIA))
        assert(isRegionalNPCZone(PORT_BASTOK))
        assert(isRegionalNPCZone(WINDURST_WOODS))
        assert(not isRegionalNPCZone(0))
        assert(not isRegionalNPCZone(231))
    end)

    it('sole first-place city shows regional NPCs', function()
        assert(showRegionalNPCs(SOUTHERN_SAN_DORIA, 1, 2, 3))
        assert(not showRegionalNPCs(PORT_BASTOK, 1, 2, 3))
        assert(not showRegionalNPCs(WINDURST_WOODS, 1, 2, 3))
        assert(showRegionalNPCs(PORT_BASTOK, 3, 1, 2))
        assert(showRegionalNPCs(WINDURST_WOODS, 2, 3, 1))
    end)

    it('first-place tie hides all regional NPCs', function()
        assert(not showRegionalNPCs(SOUTHERN_SAN_DORIA, 1, 1, 3))
        assert(not showRegionalNPCs(PORT_BASTOK, 1, 1, 3))
        assert(not showRegionalNPCs(WINDURST_WOODS, 2, 2, 2))
        assert(not showRegionalNPCs(0, 1, 2, 3))
    end)
end)
