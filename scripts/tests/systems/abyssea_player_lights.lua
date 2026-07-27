require('scripts/globals/abyssea')

describe('Abyssea player light masks', function()
    local function player(first, second)
        return {
            getCharVar = function(_, name)
                return name == 'abysseaLights1' and first or second
            end,
            setCharVar = function(_, name, value)
                if name == 'abysseaLights1' then first = value else second = value end
            end,
            masks = function() return first, second end,
        }
    end

    it('resets both player light masks', function()
        local p = player(0x04030201, 0x00070605)
        xi.abyssea.resetPlayerLights(p)
        local first, second = p.masks()
        assert(first == 0 and second == 0)
    end)

    it('currently errors when setting bonus lights through the name-keyed enum', function()
        local saved = xi.settings.main.ABYSSEA_BONUSLIGHT_AMOUNT
        xi.settings.main.ABYSSEA_BONUSLIGHT_AMOUNT = 23

        local p = player(0, 0)
        local ok = pcall(xi.abyssea.setBonusLights, p)
        xi.settings.main.ABYSSEA_BONUSLIGHT_AMOUNT = saved

        assert(not ok)
    end)
end)
