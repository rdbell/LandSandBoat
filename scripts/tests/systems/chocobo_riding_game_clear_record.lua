require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game clear record', function()
    it('clears holder and time for every route from a starting city', function()
        local starting = xi.zone.WINDURST_WOODS
        local destinations = {
            xi.zone.SAUROMUGUE_CHAMPAIGN,
            xi.zone.WEST_RONFAURE,
            xi.zone.SOUTH_GUSTABERG,
        }
        for _, destination in ipairs(destinations) do
            SetServerVariable('[ChocoGame][RecordHolder]'..starting..'+'..destination, 42)
            SetServerVariable('[ChocoGame][RecordTime]'..starting..'+'..destination, 99)
        end

        xi.chocoboGame.clearRecord({ getID = function() return starting end })

        for _, destination in ipairs(destinations) do
            assert(GetServerVariable('[ChocoGame][RecordHolder]'..starting..'+'..destination) == 0)
            assert(GetServerVariable('[ChocoGame][RecordTime]'..starting..'+'..destination) == 0)
        end
    end)
end)
