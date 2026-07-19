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
