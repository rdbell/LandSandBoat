require('scripts/globals/maws')

describe('Cavernous Maw event finish', function()
    local function player(unlocked)
        local added = {}
        local position
        local hasTeleportCalls = 0

        return {
            getZoneID = function()
                return xi.zone.BATALLIA_DOWNS_S
            end,
            hasTeleport = function(_, teleportType, bit)
                hasTeleportCalls = hasTeleportCalls + 1
                assert(teleportType == xi.teleport.type.PAST_MAW and bit == 0)
                return unlocked
            end,
            addTeleport = function(_, teleportType, bit)
                table.insert(added, { teleportType, bit })
            end,
            setPos = function(_, ...)
                position = { ... }
            end,
            added = function()
                return added
            end,
            position = function()
                return position
            end,
            hasTeleportCalls = function()
                return hasTeleportCalls
            end,
        }
    end

    it('adds an unlocked bit before travelling from an add event', function()
        local p = player(false)
        xi.maws.onEventFinish(p, 100, 1)

        assert(p.hasTeleportCalls() == 1 and #p.added() == 1 and p.added()[1][2] == 0)
        assert(p.position()[5] == xi.zone.BATALLIA_DOWNS)
    end)

    it('travels directly for a warp event without checking the unlock bit', function()
        local p = player(false)
        xi.maws.onEventFinish(p, 101, 1)

        assert(p.hasTeleportCalls() == 0 and #p.added() == 0)
        assert(p.position()[5] == xi.zone.BATALLIA_DOWNS)
    end)

    it('does nothing for an unaccepted option', function()
        local p = player(false)
        xi.maws.onEventFinish(p, 100, 0)

        assert(p.hasTeleportCalls() == 0 and #p.added() == 0 and p.position() == nil)
    end)
end)
