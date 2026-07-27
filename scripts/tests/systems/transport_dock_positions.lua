require('scripts/globals/transport')

describe('Transport dock NPC positions', function()
    it('pins the arrival and departure points for Mhaura and Selbina', function()
        local mhaura = xi.transport.dockNPCPositions(xi.zone.MHAURA)
        local selbina = xi.transport.dockNPCPositions(xi.zone.SELBINA)

        assert(#mhaura.ARRIVING == 1 and #mhaura.DEPARTING == 1)
        assert(#selbina.ARRIVING == 1 and #selbina.DEPARTING == 1)
        assert(mhaura.ARRIVING[1].x == 7.06 and mhaura.ARRIVING[1].rotation == 211)
        assert(mhaura.DEPARTING[1].x == 8.26 and mhaura.DEPARTING[1].rotation == 193)
        assert(selbina.ARRIVING[1].z == -58.843 and selbina.ARRIVING[1].rotation == 209)
        assert(selbina.DEPARTING[1].y == -1.389 and selbina.DEPARTING[1].rotation == 191)
        assert(next(xi.transport.dockNPCPositions(0)) == nil)
    end)
end)

describe('Transport dock path selection', function()
    it('uses arrival points for even triggers and departure points for odd triggers', function()
        local path
        local shown
        local npc = {
            showText = function(_, _, message)
                shown = message
            end,
            pathThrough = function(_, points)
                path = points
            end,
        }
        local messages = { [0] = 100, [1] = 101 }

        xi.transport.dockMessage(npc, 0, messages, xi.zone.MHAURA)
        assert(shown == 100 and path[1].x == 7.06 and path[1].rotation == 211)

        xi.transport.dockMessage(npc, 1, messages, xi.zone.MHAURA)
        assert(shown == 101 and path[1].x == 8.26 and path[1].rotation == 193)
    end)
end)
