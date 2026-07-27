require('scripts/globals/abyssea')

describe('Abyssea add player lights', function()
    local function player(first, second)
        local messages = {}
        return {
            getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end,
            getCharVar = function(_, name) return name == 'abysseaLights1' and first or second end,
            setCharVar = function(_, name, value)
                if name == 'abysseaLights1' then first = value else second = value end
            end,
            messageSpecial = function(_, ...) table.insert(messages, { ... }) end,
            masks = function() return first, second end,
            messages = function() return messages end,
        }
    end

    it('caps Pearl light and its message tier', function()
        local p = player(220, 0)
        xi.abyssea.addPlayerLights(p, xi.abyssea.lightType.PEARL, 50)
        local first = p.masks()
        assert(first == 230 and p.messages()[1][1] == 7521 and p.messages()[1][2] == 2)
    end)

    it('caps Azure light at 255 without reducing its allowed tier', function()
        local p = player(0, 250)
        xi.abyssea.addPlayerLights(p, xi.abyssea.lightType.AZURE, 10)
        local _, second = p.masks()
        assert(second == 255 and p.messages()[1][1] == 7525 and p.messages()[1][2] == 1)
    end)

    it('clamps a negative amount to zero', function()
        local p = player(2, 0)
        xi.abyssea.addPlayerLights(p, xi.abyssea.lightType.PEARL, -5)
        local first = p.masks()
        assert(first == 0 and p.messages()[1][2] == 0)
    end)
end)
