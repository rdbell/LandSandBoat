require('scripts/globals/teleports/survival_guide')

describe('Survival Guide update', function()
    it('adds a favorite, stores the menu, and emits its packed update', function()
        local stored, updated = nil, nil
        local player = {
            getTeleportMenu = function()
                return { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 }
            end,
            setTeleportMenu = function(_, _, favorites)
                stored = { unpack(favorites) }
            end,
            updateEvent = function(_, ...) updated = { ... } end,
        }

        xi.survivalGuide.onEventUpdate(player, 8500, 0x002A0004)

        assert(stored[1] == 42 and stored[2] == 1 and stored[9] == 8 and stored[10] == 0)
        assert(updated[1] == 0x0302012A and updated[2] == 0x07060504 and updated[3] == 8)
    end)
end)

describe('Survival Guide finish', function()
    it('does not teleport when the player is farther than six yalms from the guide', function()
        local player = {
            getZoneID = function() return 999 end,
            checkDistance = function() return 6.1 end,
            hasTeleport = function() return true end,
            setPos = function() error('distant player must not teleport') end,
        }

        xi.survivalGuide.onEventFinish(player, 8500, 1, {})
    end)
end)
