-- Focused characterization for xi.escha.portals.eschanPortalOnTrigger.
require('scripts/globals/teleports/eschan_portals')

local function fakePlayer(zone, mask, keyItem, tempItems, silt)
    local calls = {}
    local player =
    {
        getTeleport = function() return mask end,
        getZoneID = function() return zone end,
        hasKeyItem = function(_, item) return item == keyItem end,
        hasItem = function(_, item) return tempItems[item] == true end,
        addTeleport = function(_, _, portalBit)
            mask = bit.bor(mask, bit.lshift(1, portalBit))
            calls[#calls + 1] = { name = 'addTeleport', bit = portalBit }
        end,
        messageSpecial = function(_, key) calls[#calls + 1] = { name = 'messageSpecial', key = key } end,
        getCurrency = function(_, currency)
            assert(currency == 'escha_silt')
            return silt
        end,
        startEvent = function(_, event, ...)
            calls[#calls + 1] = { name = 'startEvent', event = event, params = { ... } }
        end,
    }
    return player, calls
end

local function fakeNPC(name)
    return { getName = function() return name end }
end

describe('Eschan portal trigger 9241', function()
    it('preserves uint32 bit 31 and all eight event parameters', function()
        local player, calls = fakePlayer(xi.zone.REISENJIMA, bit.bor(bit.lshift(1, 30), bit.lshift(1, 31)), nil, {}, 123)
        xi.escha.portals.eschanPortalOnTrigger(player, fakeNPC('Ethereal_Ingress_#9'))

        assert(#calls == 1 and calls[1].name == 'startEvent' and calls[1].event == 9100)
        local p = calls[1].params
        assert(#p == 8 and p[1] == 0 and p[2] == bit.bor(bit.lshift(1, 30), bit.lshift(1, 31)) and p[3] == xi.zone.REISENJIMA)
        assert(p[4] == 31 and p[5] == 0 and p[6] == 123 and p[7] == 50 and p[8] == 0)
    end)

    it('uses first-menu mask after unlocking the first ZiTah portal', function()
        local player, calls = fakePlayer(xi.zone.ESCHA_ZITAH, 0, nil, {}, 8)
        xi.escha.portals.eschanPortalOnTrigger(player, fakeNPC('Eschan_Portal_#1'))

        assert(calls[1].name == 'addTeleport' and calls[1].bit == 0)
        local p = calls[2].params
        assert(p[2] == 1 and p[4] == 0 and p[5] == 1 and p[6] == 8 and p[7] == 50)
    end)

    it('returns the Ru\'Aun blind message when no portal is unlocked', function()
        local player, calls = fakePlayer(xi.zone.ESCHA_RUAUN, 0, nil, {}, 0)
        xi.escha.portals.eschanPortalOnTrigger(player, fakeNPC('Eschan_Portal_#1'))

        assert(#calls == 1 and calls[1].name == 'messageSpecial' and calls[1].key == nil)
    end)
end)
