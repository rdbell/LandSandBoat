-- Focused characterization for xi.escha.portals.eschanPortalEventFinish.
require('scripts/globals/teleports/eschan_portals')

local function fakePlayer(zone, items)
    local calls = {}
    local player =
    {
        getZoneID = function() return zone end,
        hasItem = function(_, item) return items[item] == true end,
        delItem = function(_, item, count, inventory)
            calls[#calls + 1] = { name = 'delItem', item = item, count = count, inventory = inventory }
        end,
        messageSpecial = function(_, key, item)
            calls[#calls + 1] = { name = 'messageSpecial', key = key, item = item }
        end,
        delCurrency = function(_, currency, count)
            calls[#calls + 1] = { name = 'delCurrency', currency = currency, count = count }
        end,
    }
    return player, calls
end

describe('Eschan portal event finish 9240', function()
    it('consumes the Reisenjima droplet before messaging', function()
        local player, calls = fakePlayer(xi.zone.REISENJIMA, { [xi.item.ETHEREAL_DROPLET] = true })
        xi.escha.portals.eschanPortalEventFinish(player, 0, 3, nil)

        assert(#calls == 2)
        assert(calls[1].name == 'delItem' and calls[1].item == 9202 and calls[1].count == 1)
        assert(calls[1].inventory == xi.inv.TEMPITEMS)
        assert(calls[2].name == 'messageSpecial' and calls[2].key == 7622 and calls[2].item == 9202)
    end)

    it('uses the Ru\'Aun item branch for options 2 and 3, preserving its missing message key', function()
        for _, option in ipairs({ 2, 3 }) do
            local player, calls = fakePlayer(xi.zone.ESCHA_RUAUN, { [xi.item.CLUMP_OF_ESCHAN_DROPLETS] = true })
            xi.escha.portals.eschanPortalEventFinish(player, 0, option, nil)

            assert(#calls == 2)
            assert(calls[1].name == 'delItem' and calls[1].item == 9148 and calls[1].count == 1)
            assert(calls[1].inventory == xi.inv.TEMPITEMS)
            assert(calls[2].name == 'messageSpecial' and calls[2].key == nil and calls[2].item == 9148)
        end
    end)

    it('debits 50 silt only for non-item options other than cancel choices', function()
        for _, option in ipairs({ 1, 5, 0x40000001 }) do
            local player, calls = fakePlayer(xi.zone.ESCHA_ZITAH, {})
            xi.escha.portals.eschanPortalEventFinish(player, 0, option, nil)

            assert(#calls == 1 and calls[1].name == 'delCurrency')
            assert(calls[1].currency == 'escha_silt' and calls[1].count == 50)
        end
        for _, option in ipairs({ 0, 4, 0x40000000 }) do
            local player, calls = fakePlayer(xi.zone.ESCHA_ZITAH, {})
            xi.escha.portals.eschanPortalEventFinish(player, 0, option, nil)
            assert(#calls == 0)
        end
    end)
end)
