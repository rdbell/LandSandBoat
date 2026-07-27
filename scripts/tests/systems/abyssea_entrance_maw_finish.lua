require('scripts/globals/abyssea')

describe('Abyssea entrance maw finish', function()
    local function player()
        local position
        return {
            getZoneID = function() return xi.zone.LA_THEINE_PLATEAU end,
            setPos = function(_, ...) position = { ... } end,
            position = function() return position end,
        }
    end

    it('warps only for the accepted route event', function()
        local p = player()
        xi.abyssea.entranceMawOnEventFinish(p, 218, 1)
        assert(p.position()[1] == -480 and p.position()[5] == xi.zone.ABYSSEA_LA_THEINE)
    end)

    it('does not warp for a declined event', function()
        local p = player()
        xi.abyssea.entranceMawOnEventFinish(p, 218, 0)
        assert(p.position() == nil)
    end)
end)
