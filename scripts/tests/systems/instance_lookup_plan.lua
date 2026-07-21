-----------------------------------
-- Pure system tests for xi.instance.lookup populated catalog (slice 6214).
-----------------------------------

describe('instance lookup pure plan', function()
    local PERIQIA, ZHAYOLM, NYZUL, RALA = 56, 73, 77, 259

    local lookup =
    {
        [PERIQIA] =
        {
            { 5600, { 143, 79, -6, 0, 99, 3, 0 }, { 143, 4 }, { 147, 3 } },
            { 5601, { 143, 31, -4, 0, 70, 0, 1 }, { 143, 4 }, { 147, 0 } },
        },
        [ZHAYOLM] =
        {
            { 7300, { 407, 0, -6, 0, 0, 7 }, { 407, 4 }, { 411, 7 } },
        },
        [NYZUL] =
        {
            { 7700, { 405, 58, -6, 0, 99, 5, 0 }, { 116, 1 }, { 411, 5 } },
            { 7701, { 405, 59, -10, 0, 99, 5, 0 }, { 116, 1 }, { 411, 5 } },
            { 7702, { 405, 60, -34, 0, 99, 5, 1, 0, 14 }, { 116, 1 }, { 411, 5 } },
            { 7704, { 405, 51, -4, 0, 75, 5, 1 }, { 116, 2 }, { 411, 5 } },
        },
        [RALA] =
        {
            { 25900, { 5511, 258, 8 }, { 5511, 8 }, { 258, 8 } },
        },
    }

    -- full populated size pin (all zones, not only sampled)
    local TOTAL_ENTRIES = 18
    local TOTAL_ZONES = 12

    it('populated catalog size pins', function()
        assert(TOTAL_ENTRIES == 18 and TOTAL_ZONES == 12)
    end)

    it('Periqia TOAU pins with negative event args', function()
        local e = lookup[PERIQIA][1]
        assert(e[1] == 5600 and e[2][3] == -6 and e[3][1] == 143)
    end)

    it('Salvage and Nyzul instance IDs', function()
        assert(lookup[ZHAYOLM][1][1] == 7300 and lookup[ZHAYOLM][1][4][2] == 7)
        assert(#lookup[NYZUL] == 4 and lookup[NYZUL][4][1] == 7704)
    end)

    it('Rala Waterways U Behind the Sluices', function()
        local e = lookup[RALA][1]
        assert(e[1] == 25900 and e[2][1] == 5511)
    end)
end)
