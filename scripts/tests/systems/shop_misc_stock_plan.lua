-----------------------------------
-- Pure system tests for outpost/celebratory/Valeriano stock + nation filter (slice 6210).
-----------------------------------

describe('shop misc stock pure plan', function()
    local SANDORIA, BASTOK = 0, 1
    local SOUTHERN_SAN_DORIA, PORT_BASTOK, WINDURST_WOODS = 230, 236, 241

    local outpost =
    {
        { 4148, 316 },  -- ANTIDOTE
        { 4151, 800 },  -- ECHO DROPS
        { 4128, 4832 }, -- ETHER
        { 4150, 2595 }, -- EYE DROPS
        { 4112, 910 },  -- POTION
    }

    local celebratory =
    {
        { 4167, 30 }, { 4168, 30 }, { 4215, 60 }, { 4216, 60 },
        { 4256, 30 }, { 4169, 30 }, { 5769, 650 }, { 4170, 1000 },
        { 5424, 6000 }, { 5425, 6000 }, { 4441, 1116 },
        { 4238, 3000 }, { 4240, 3000 }, { 4241, 3000 },
    }

    local valeriano =
    {
        { 4394, 12 }, { 17345, 49 }, { 17347, 1144 },
        { 5017, 677 }, { 5018, 19552 }, { 5013, 3369 },
        { 5027, 2379 }, { 5072, 104000 },
        { 5054, 37128 }, { 5056, 34944 }, { 5057, 30680 },
        { 5059, 32240 }, { 4996, 140039 },
    }

    local valZones =
    {
        [SOUTHERN_SAN_DORIA] = { nation = 0, fame = 0 },
        [PORT_BASTOK] = { nation = 1, fame = 1 },
        [WINDURST_WOODS] = { nation = 2, fame = 2 },
    }

    local function nationItem(tier, playerNation, shopNation, rank)
        if tier == 1 then
            return playerNation == shopNation and rank == 1
        elseif tier == 2 then
            return rank <= 2
        elseif tier == 3 then
            return true
        end
        return false
    end

    local function valerianoOpen(zoneId, rank)
        local z = valZones[zoneId]
        if not z then return nil end
        return rank == 1, z.fame
    end

    it('outpost stock size and pins', function()
        assert(#outpost == 5)
        assert(outpost[1][1] == 4148 and outpost[1][2] == 316)
        assert(outpost[3][1] == 4128 and outpost[3][2] == 4832)
        assert(outpost[5][1] == 4112 and outpost[5][2] == 910)
    end)

    it('celebratory stock size and crystal pins', function()
        assert(#celebratory == 14)
        assert(celebratory[1][2] == 30 and celebratory[9][2] == 6000)
        assert(celebratory[12][1] == 4238 and celebratory[12][2] == 3000)
    end)

    it('Valeriano stock and zone table', function()
        assert(#valeriano == 13)
        assert(valeriano[1][1] == 4394 and valeriano[13][2] == 140039)
        assert(valZones[SOUTHERN_SAN_DORIA].nation == 0)
        assert(valZones[PORT_BASTOK].fame == 1)
    end)

    it('nation stock tier filter', function()
        assert(nationItem(1, SANDORIA, SANDORIA, 1))
        assert(not nationItem(1, BASTOK, SANDORIA, 1))
        assert(not nationItem(1, SANDORIA, SANDORIA, 2))
        assert(nationItem(2, BASTOK, SANDORIA, 2))
        assert(not nationItem(2, BASTOK, SANDORIA, 3))
        assert(nationItem(3, BASTOK, SANDORIA, 10))
    end)

    it('Valeriano open only when nation rank is 1', function()
        local open, fame = valerianoOpen(SOUTHERN_SAN_DORIA, 1)
        assert(open and fame == 0)
        open, fame = valerianoOpen(PORT_BASTOK, 2)
        assert(not open and fame == 1)
        assert(valerianoOpen(0, 1) == nil)
    end)
end)
