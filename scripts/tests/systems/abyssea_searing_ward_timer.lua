require('scripts/globals/abyssea')

describe('Abyssea searing ward timer', function()
    local function player(timer)
        local messages = {}
        local timers = {}
        local position

        return {
            getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end,
            getLocalVar = function() return timer end,
            setLocalVar = function(_, _, value) timer = value end,
            messageSpecial = function(_, ...) table.insert(messages, { ... }) end,
            timer = function(_, delay) table.insert(timers, delay) end,
            setPos = function(_, ...) position = { ... } end,
            value = function() return timer end,
            messages = function() return messages end,
            timers = function() return timers end,
            position = function() return position end,
        }
    end

    it('warns and reschedules during the final six ticks', function()
        local p = player(6)
        xi.abyssea.searingWardTimer(p)
        assert(p.value() == 5 and p.messages()[1][2] == 5 and p.timers()[1] == 1500)
    end)

    it('resets and teleports on the final tick', function()
        local p = player(1)
        xi.abyssea.searingWardTimer(p)
        assert(p.value() == 0 and #p.messages() == 1 and #p.timers() == 0)
        assert(p.position()[1] == 396 and p.position()[3] == 276)
    end)
end)
