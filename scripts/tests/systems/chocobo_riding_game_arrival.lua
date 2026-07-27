require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game arrival', function()
    it('stages completion, starts the route event, and replaces an empty record', function()
        local starting = xi.zone.WINDURST_WOODS
        local destination = xi.zone.SAUROMUGUE_CHAMPAIGN
        local holderKey = '[ChocoGame][RecordHolder]'..starting..'+'..destination
        local timeKey = '[ChocoGame][RecordTime]'..starting..'+'..destination
        SetServerVariable(holderKey, 0)
        SetServerVariable(timeKey, 0)

        local clearTime
        local event
        local player = {
            getCharVar = function(_, key)
                if key == '[ChocoGame]DestCity' then return destination end
                if key == '[ChocoGame]StartingCity' then return starting end
                if key == '[ChocoGame]StartTime' then return GetSystemTime() - 45 end
            end,
            getZoneID = function() return destination end,
            setCharVar = function(_, key, value)
                assert(key == '[ChocoGame]ClearTime')
                clearTime = value
            end,
            startEvent = function(_, ...)
                event = { ... }
            end,
            getID = function() return 424242 end,
        }

        local ok, err = pcall(function()
            xi.chocoboGame.onTriggerAreaEnter(player)
            assert(clearTime and clearTime >= 45 and clearTime <= 47)
            assert(event and event[1] == 901 and event[2] == clearTime)
            assert(GetServerVariable(holderKey) == 424242)
            assert(GetServerVariable(timeKey) == clearTime)
        end)
        SetServerVariable(holderKey, 0)
        SetServerVariable(timeKey, 0)
        assert(ok, err)
    end)

    it('does nothing before reaching the destination zone', function()
        local called = false
        local player = {
            getCharVar = function() return xi.zone.SAUROMUGUE_CHAMPAIGN end,
            getZoneID = function() return xi.zone.WINDURST_WOODS end,
            setCharVar = function() called = true end,
        }
        xi.chocoboGame.onTriggerAreaEnter(player)
        assert(not called)
    end)
end)
