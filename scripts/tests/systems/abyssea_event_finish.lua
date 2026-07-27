require('scripts/globals/abyssea')

describe('Abyssea event finish', function()
    local function player(zoneID)
        local position

        return {
            getZoneID = function()
                return zoneID
            end,
            setPos = function(_, ...)
                position = { ... }
            end,
            position = function()
                return position
            end,
        }
    end

    it('moves to the current zone exit for the exit event', function()
        local p = player(xi.zone.ABYSSEA_ALTEPA)
        xi.abyssea.onEventFinish(p, 2180, 0)

        local pos = p.position()
        assert(pos[1] == 340 and pos[2] == -0.52 and pos[3] == -668)
        assert(pos[4] == 192 and pos[5] == xi.zone.SOUTH_GUSTABERG)
    end)

    it('does not move for another event', function()
        local p = player(xi.zone.ABYSSEA_ALTEPA)
        xi.abyssea.onEventFinish(p, 2179, 0)

        assert(p.position() == nil)
    end)
end)
