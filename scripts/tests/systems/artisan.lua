require('scripts/globals/artisan')

describe('Artisan Moogle trigger', function()
    it('starts the first-visit menu with sack purchase flags', function()
        local event = nil
        local visited = nil
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS end,
            getContainerSize = function() return 0 end,
            getCharVar = function() return 0 end,
            setCharVar = function(_, name, value) visited = { name, value } end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.artisan.moogleOnTrigger(player, {})

        assert(event[1] == 544 and event[5] == 0 and event[8] == 10 and event[9] == 0)
        assert(visited[1] == '[artisan]visited' and visited[2] == 1)
    end)
end)

describe('Artisan Moogle sack purchase', function()
    it('buys a Mog Sack and refreshes the menu', function()
        local debited = nil
        local size = nil
        local visited = nil
        local event = nil
        local player = {
            getGil = function() return 9980 end,
            getContainerSize = function() return 0 end,
            delGil = function(_, amount) debited = amount end,
            changeContainerSize = function(_, _, amount) size = amount end,
            setCharVar = function(_, name, value) visited = { name, value } end,
            updateEvent = function(_, ...) event = { ... } end,
        }

        xi.artisan.moogleOnUpdate(player, 544, 1, {})

        assert(debited == 9980 and size == 30)
        assert(visited[1] == '[artisan]visited' and visited[2] == 0)
        assert(event[4] == 31 and event[8] == 2)
    end)
end)

describe('Artisan Moogle sack expansion', function()
    it('expands the Mog Sack to inventory size and refreshes progress', function()
        local sackSize = 30
        local event = nil
        local player = {
            getContainerSize = function(_, container)
                return container == xi.inv.MOGSACK and sackSize or 35
            end,
            changeContainerSize = function(_, _, amount) sackSize = sackSize + amount end,
            updateEvent = function(_, ...) event = { ... } end,
        }

        xi.artisan.moogleOnUpdate(player, 544, 2, {})

        assert(sackSize == 35)
        assert(event[1] == 1 and event[4] == 36 and event[7] == 2 and event[8] == 0)
    end)
end)

describe('Artisan Moogle scroll finish', function()
    it('grants the daily scroll and records the claim timestamp', function()
        local nextScroll = nil
        local given = nil
        local player = {
            getCharVar = function() return 0 end,
            setCharVar = function(_, name, value) nextScroll = { name, value } end,
        }
        local oldGiveItem = npcUtil.giveItem
        npcUtil.giveItem = function(_, item) given = item; return true end

        xi.artisan.moogleOnFinish(player, 544, 99, {})

        npcUtil.giveItem = oldGiveItem
        assert(given == xi.item.SCROLL_OF_INSTANT_WARP)
        assert(nextScroll[1] == '[artisan]nextScroll' and nextScroll[2] > 0)
    end)
end)

describe('Artisan Moogle main dialogue', function()
    it('updates gil, sack display, and scroll availability', function()
        local event = nil
        local player = {
            getCharVar = function() return 0 end,
            getContainerSize = function() return 30 end,
            getGil = function() return 1234 end,
            updateEvent = function(_, ...) event = { ... } end,
        }

        xi.artisan.moogleOnUpdate(player, 544, 4, {})

        assert(event[3] == 1234 and event[4] == 31 and event[8] == 1)
    end)
end)
