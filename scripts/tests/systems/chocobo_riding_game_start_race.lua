require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game start-race event', function()
    local function player(zone)
        local stagedDestination
        local event
        return {
            getZone = function()
                return { getID = function() return zone end }
            end,
            setCharVar = function(_, key, value)
                assert(key == '[ChocoGame]DestCity')
                stagedDestination = value
            end,
            startEvent = function(_, ...)
                event = { ... }
            end,
            stagedDestination = function() return stagedDestination end,
            event = function() return event end,
        }
    end

    it('stages each route destination and emits its event parameter', function()
        local cases = {
            { xi.zone.WINDURST_WOODS, xi.zone.SAUROMUGUE_CHAMPAIGN, 3 },
            { xi.zone.WINDURST_WOODS, xi.zone.WEST_RONFAURE, 0 },
            { xi.zone.WINDURST_WOODS, xi.zone.SOUTH_GUSTABERG, 1 },
            { xi.zone.BASTOK_MINES, xi.zone.EAST_SARUTABARUTA, 2 },
            { xi.zone.BASTOK_MINES, xi.zone.ROLANBERRY_FIELDS, 3 },
            { xi.zone.BASTOK_MINES, xi.zone.WEST_RONFAURE, 0 },
            { xi.zone.SOUTHERN_SAN_DORIA, xi.zone.SOUTH_GUSTABERG, 1 },
            { xi.zone.SOUTHERN_SAN_DORIA, xi.zone.EAST_SARUTABARUTA, 2 },
            { xi.zone.SOUTHERN_SAN_DORIA, xi.zone.BATALLIA_DOWNS, 3 },
            { xi.zone.KAZHAM, xi.zone.YUHTUNGA_JUNGLE, 3 },
        }
        for _, case in ipairs(cases) do
            local p = player(case[1])
            xi.chocoboGame.startRaceEvent(p, case[2], 777)
            assert(p.stagedDestination() == case[2])
            assert(p.event()[1] == 777 and p.event()[2] == -3 and p.event()[3] == 0 and p.event()[4] == 0 and p.event()[5] == case[3])
        end
    end)
end)
