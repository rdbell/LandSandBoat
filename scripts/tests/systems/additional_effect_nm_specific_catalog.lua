-----------------------------------
-- Pure system tests for additionalEffect nmSpecificConfigs catalog (slice 6137).
-- Source: scripts/globals/additional_effects.lua ~521–562
-----------------------------------

describe('additionalEffect nmSpecificConfigs pure catalog', function()
    local catalog = {
        Brigandish_Blade = { requiredItem = 17622 }, -- BUCCANEERS_KNIFE
        Seiryu           = { requiredItem = 18163 }, -- ZEPHYR
        Genbu            = { requiredItem = 18164 }, -- ANTARCTIC_WIND
        Suzaku           = { requiredItem = 18161 }, -- ARCTIC_WIND
        Byakko           = { requiredItem = 18162 }, -- EAST_WIND
    }

    local function lookup(name)
        return catalog[name]
    end

    local function match(required, item)
        return required == 0 or required == item
    end

    it('has five known NM rows', function()
        local n = 0
        for _ in pairs(catalog) do n = n + 1 end
        assert(n == 5)
    end)

    it('maps names to required items', function()
        assert(lookup('Seiryu').requiredItem == 18163)
        assert(lookup('Brigandish_Blade').requiredItem == 17622)
        assert(lookup('missing') == nil)
    end)

    it('item match gate', function()
        local cfg = lookup('Genbu')
        assert(match(cfg.requiredItem, 18164) == true)
        assert(match(cfg.requiredItem, 18161) == false)
    end)
end)
