require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game begin race', function()
    local function player(zone, destination)
        local writes = {}
        return {
            getZone = function()
                return { getID = function() return zone end }
            end,
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]DestCity')
                return destination
            end,
            setCharVar = function(_, key, value, expiry)
                writes[#writes + 1] = { key = key, value = value, expiry = expiry }
            end,
            writes = function() return writes end,
        }
    end

    it('acceptance writes route, weekly lockout, and start time', function()
        local p = player(xi.zone.WINDURST_WOODS, xi.zone.SAUROMUGUE_CHAMPAIGN)
        xi.chocoboGame.beginRace(p, 0)
        local writes = p.writes()
        assert(#writes == 4)
        assert(writes[1].key == '[ChocoGame]StartingCity' and writes[1].value == xi.zone.WINDURST_WOODS)
        assert(writes[2].key == '[ChocoGame]DestCity' and writes[2].value == xi.zone.SAUROMUGUE_CHAMPAIGN)
        assert(writes[3].key == '[ChocoGame]NextEntryTime' and writes[3].value == 1 and writes[3].expiry > 0)
        assert(writes[4].key == '[ChocoGame]StartTime' and writes[4].value > 0)
    end)

    it('declining clears only the staged destination', function()
        local p = player(xi.zone.WINDURST_WOODS, xi.zone.SAUROMUGUE_CHAMPAIGN)
        xi.chocoboGame.beginRace(p, 1)
        local writes = p.writes()
        assert(#writes == 1)
        assert(writes[1].key == '[ChocoGame]DestCity' and writes[1].value == 0 and writes[1].expiry == nil)
    end)
end)
