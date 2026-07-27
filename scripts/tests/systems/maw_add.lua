require('scripts/globals/maws')

describe('Cavernous Maw add', function()
    local function player(unlocked)
        local added = {}
        local position

        return {
            getZoneID = function()
                return xi.zone.BATALLIA_DOWNS
            end,
            hasTeleport = function(_, teleportType, bit)
                assert(teleportType == xi.teleport.type.PAST_MAW)
                assert(bit == 0)
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
        }
    end

    it('unlocks a current-zone maw before moving to its past destination', function()
        local p = player(false)
        xi.maws.addMaw(p)

        assert(#p.added() == 1)
        assert(p.added()[1][2] == 0)
        local position = p.position()
        assert(position[1] == -51.486 and position[2] == 0.371 and position[3] == 436.972)
        assert(position[4] == 128 and position[5] == 84)
    end)

    it('does not add an already unlocked explicit maw', function()
        local p = player(true)
        xi.maws.addMaw(p, xi.maws.pastMaws[xi.zone.BATALLIA_DOWNS])

        assert(#p.added() == 0)
        local position = p.position()
        assert(position[1] == -51.486 and position[2] == 0.371 and position[3] == 436.972)
        assert(position[4] == 128 and position[5] == 84)
    end)
end)
