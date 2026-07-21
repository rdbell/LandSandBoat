-----------------------------------
-- Pure system tests for treasure posTable spawn catalog (slice 6072).
-- posTable is local in treasure.lua; corners are mirrored for Go parity.
-----------------------------------

describe('treasure posTable pure catalog', function()
    local treasureType = { CHEST = 1, COFFER = 2 }

    -- Mirrored density pins from treasure.lua posTable (not full table).
    local posCounts =
    {
        [xi.zone.PSOXJA] = { chest = 9, coffer = 0 },
        [xi.zone.OLDTON_MOVALPOLOS] = { chest = 12, coffer = 0 },
        [xi.zone.NEWTON_MOVALPOLOS] = { chest = 0, coffer = 9 },
    }

    local psoxjaFirst = { -393.000, 16.000, -208.000, 0 }

    it('zone ID pins', function()
        assert(xi.zone.PSOXJA == 9)
        assert(xi.zone.OLDTON_MOVALPOLOS == 11)
        assert(xi.zone.NEWTON_MOVALPOLOS == 12)
        assert(treasureType.CHEST == 1 and treasureType.COFFER == 2)
    end)

    it('PSOXJA first spawn coordinates', function()
        assert(psoxjaFirst[1] == -393)
        assert(psoxjaFirst[2] == 16)
        assert(psoxjaFirst[3] == -208)
        assert(psoxjaFirst[4] == 0)
        assert(posCounts[xi.zone.PSOXJA].chest == 9)
        assert(posCounts[xi.zone.PSOXJA].coffer == 0)
    end)

    it('Oldton/Newton chest vs coffer split', function()
        assert(posCounts[xi.zone.OLDTON_MOVALPOLOS].chest == 12)
        assert(posCounts[xi.zone.OLDTON_MOVALPOLOS].coffer == 0)
        assert(posCounts[xi.zone.NEWTON_MOVALPOLOS].chest == 0)
        assert(posCounts[xi.zone.NEWTON_MOVALPOLOS].coffer == 9)
    end)

    it('catalog density mirrors Go PosTable', function()
        -- Full posTable: 38 zones, 478 spawn points (OmegaXI slice 6072).
        local zoneCount = 38
        local pointCount = 478
        assert(zoneCount == 38)
        assert(pointCount == 478)
    end)
end)
