require('scripts/globals/abyssea')

describe('Abyssea searing ward trigger area', function()
    local function player(icon, timer)
        local messages = {}

        return {
            getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end,
            getStatusEffect = function()
                return icon and { getIcon = function() return icon end } or nil
            end,
            messageName = function(_, ...) table.insert(messages, { ... }) end,
            setLocalVar = function(_, _, value) timer = value end,
            value = function() return timer end,
            messages = function() return messages end,
        }
    end

    it('starts the tether countdown when hidden Visitant leaves the ward', function()
        local p = player(0, 0)
        xi.abyssea.onWardTriggerAreaLeave(p)
        assert(p.value() == 11 and p.messages()[1][1] == 7357 and p.messages()[1][3] == 10)
    end)

    it('does not tether a player with a visible or missing Visitant effect', function()
        local visible = player(1, 4)
        xi.abyssea.onWardTriggerAreaLeave(visible)
        assert(visible.value() == 4 and #visible.messages() == 0)

        local missing = player(nil, 4)
        xi.abyssea.onWardTriggerAreaLeave(missing)
        assert(missing.value() == 4 and #missing.messages() == 0)
    end)

    it('cancels the tether countdown when entering the ward', function()
        local p = player(nil, 11)
        xi.abyssea.onWardTriggerAreaEnter(p)
        assert(p.value() == 0)
    end)
end)
