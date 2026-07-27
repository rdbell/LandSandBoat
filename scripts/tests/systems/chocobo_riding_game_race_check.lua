require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game race admission', function()
    local function player(zone, nextEntryTime)
        return {
            getZoneID = function() return zone end,
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]NextEntryTime')
                return nextEntryTime
            end,
        }
    end

    local function npc(id)
        return { getID = function() return id end }
    end

    it('matches each city active route to its Vana day and offering NPC', function()
        local day = math.floor(VanadielTime() / 3456) % 3
        local cases = {
            { xi.zone.WINDURST_WOODS, { zones[xi.zone.WINDURST_WOODS].npc.ORLAINE, zones[xi.zone.WINDURST_WOODS].npc.SARIALE, zones[xi.zone.WINDURST_WOODS].npc.AMIMI }, { xi.zone.SAUROMUGUE_CHAMPAIGN, xi.zone.WEST_RONFAURE, xi.zone.SOUTH_GUSTABERG } },
            { xi.zone.BASTOK_MINES, { zones[xi.zone.BASTOK_MINES].npc.AZETTE, zones[xi.zone.BASTOK_MINES].npc.EULAPHE, zones[xi.zone.BASTOK_MINES].npc.QUELLE }, { xi.zone.EAST_SARUTABARUTA, xi.zone.ROLANBERRY_FIELDS, xi.zone.WEST_RONFAURE } },
            { xi.zone.SOUTHERN_SAN_DORIA, { zones[xi.zone.SOUTHERN_SAN_DORIA].npc.CAMEREINE, zones[xi.zone.SOUTHERN_SAN_DORIA].npc.EMOUSSINE, zones[xi.zone.SOUTHERN_SAN_DORIA].npc.MEUNEILLE }, { xi.zone.SOUTH_GUSTABERG, xi.zone.EAST_SARUTABARUTA, xi.zone.BATALLIA_DOWNS } },
        }
        for _, case in ipairs(cases) do
            assert(xi.chocoboGame.raceCheck(player(case[1], 0), npc(case[2][day + 1])) == case[3][day + 1])
        end
        assert(xi.chocoboGame.raceCheck(player(xi.zone.KAZHAM, 0), npc(zones[xi.zone.KAZHAM].npc.TIELLEQUE)) == (day == 0 and xi.zone.YUHTUNGA_JUNGLE or false))
    end)

    it('rejects a wrong NPC, previous weekly entry, and a non-racing zone', function()
        local day = math.floor(VanadielTime() / 3456) % 3
        local npcs = { zones[xi.zone.WINDURST_WOODS].npc.ORLAINE, zones[xi.zone.WINDURST_WOODS].npc.SARIALE, zones[xi.zone.WINDURST_WOODS].npc.AMIMI }
        assert(xi.chocoboGame.raceCheck(player(xi.zone.WINDURST_WOODS, 0), npc(npcs[(day + 1) % 3 + 1])) == false)
        assert(xi.chocoboGame.raceCheck(player(xi.zone.WINDURST_WOODS, 1), npc(npcs[day + 1])) == false)
        assert(xi.chocoboGame.raceCheck(player(xi.zone.RULUDE_GARDENS, 0), npc(0)) == false)
    end)
end)
