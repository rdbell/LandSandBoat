require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game elapsed message', function()
    local function player(startTime)
        local message
        return {
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]StartTime')
                return startTime
            end,
            getZoneID = function() return xi.zone.WINDURST_WOODS end,
            messageSpecial = function(_, ...)
                message = { ... }
            end,
            message = function() return message end,
        }
    end

    it('reports elapsed time only after 30 seconds of a race', function()
        local now = GetSystemTime()
        local notRacing = player(0)
        xi.chocoboGame.handleMessage(notRacing)
        assert(notRacing.message() == nil)

        local justStarted = player(now - 30)
        xi.chocoboGame.handleMessage(justStarted)
        assert(justStarted.message() == nil)

        local racing = player(now - 31)
        xi.chocoboGame.handleMessage(racing)
        local message = racing.message()
        assert(message and message[1] == zones[xi.zone.WINDURST_WOODS].text.TIME_ELAPSED)
        assert(message[2] == 0 and message[3] == 0 and message[4] >= 31 and message[4] <= 33)
    end)
end)
