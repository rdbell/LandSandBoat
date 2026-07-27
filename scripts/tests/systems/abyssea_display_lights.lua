require('scripts/globals/abyssea')

describe('Abyssea display lights', function()
    local function player(region)
        local messages = {}
        return {
            getCurrentRegion = function() return region end,
            getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end,
            getCharVar = function(_, name) return name == 'abysseaLights1' and 0x04030201 or 0x00070605 end,
            messageName = function(_, ...) table.insert(messages, { ... }) end,
            messages = function() return messages end,
        }
    end

    it('sends the two light messages in source enum order inside Abyssea', function()
        local p = player(xi.region.ABYSSEA)
        xi.abyssea.displayAbysseaLights(p)
        assert(#p.messages() == 2)
        assert(p.messages()[1][1] == 7338 and p.messages()[1][3] == 1 and p.messages()[1][4] == 4 and p.messages()[1][5] == 2 and p.messages()[1][6] == 3)
        assert(p.messages()[2][1] == 7339 and p.messages()[2][3] == 5 and p.messages()[2][4] == 6 and p.messages()[2][5] == 7)
    end)

    it('does not send light messages outside Abyssea', function()
        local p = player(0)
        xi.abyssea.displayAbysseaLights(p)
        assert(#p.messages() == 0)
    end)
end)
