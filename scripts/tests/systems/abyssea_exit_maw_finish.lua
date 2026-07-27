require('scripts/globals/abyssea')

describe('Abyssea exit maw finish', function()
    local function player()
        local position
        return {
            getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end,
            setPos = function(_, ...) position = { ... } end,
            position = function() return position end,
        }
    end

    it('warps only for an accepted exit event', function()
        local p = player()
        xi.abyssea.exitMawOnEventFinish(p, 200, 1)
        assert(p.position()[1] == 343 and p.position()[5] == xi.zone.SOUTH_GUSTABERG)
    end)

    it('does not warp when the exit event is declined', function()
        local p = player()
        xi.abyssea.exitMawOnEventFinish(p, 200, 0)
        assert(p.position() == nil)
    end)
end)
