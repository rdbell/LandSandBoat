require('scripts/globals/title_changer')

describe('Title Changer events', function()
    it('starts an event with six title masks, its flag, and player gil', function()
        local event = nil
        local player = {
            hasTitle = function(_, title) return title == 11 end,
            getGil = function() return 500 end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local info = {
            { cost = 10, title = { 11, 12 } },
            { cost = 20, title = { 21 } },
            {}, {}, {}, {},
        }

        xi.titleChanger.onTrigger(player, 700, info)

        assert(event[1] == 700 and event[2] == 0x1FFFFFFC)
        assert(event[3] == 0x1FFFFFFE and event[8] == 1 and event[9] == 500)
    end)

    it('sets a selected title only after its gil debit succeeds', function()
        local title, debits = nil, 0
        local player = {
            delGil = function(_, cost) debits = debits + cost; return true end,
            setTitle = function(_, value) title = value end,
        }
        local info = { { cost = 100, title = { 44 } } }

        xi.titleChanger.onEventFinish(player, 700, 1, 700, info)
        assert(title == 44 and debits == 100)

        player.delGil = function() return false end
        title = nil
        xi.titleChanger.onEventFinish(player, 700, 1, 700, info)
        assert(title == nil)
    end)
end)
