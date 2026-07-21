-----------------------------------
-- Pure system tests for steamJacket / scarletDelirium multipliers (slice 6075).
-- Entity hosts are mocked via pure inject mirrors.
-----------------------------------

describe('damage multipliers steam jacket pure inject', function()
    local function steamJacketMultiplier(magicalElement, reductionMod, priorTracked)
        if magicalElement < xi.element.FIRE or magicalElement > xi.element.DARK then
            return 1, false, nil
        end
        if reductionMod <= 0 then
            return 1, false, nil
        end
        local write = true
        local newTracked = magicalElement
        if priorTracked ~= magicalElement then
            return 1, write, newTracked
        end
        return 1 - reductionMod / 100, write, newTracked
    end

    local function scarletDeliriumMultiplier(hasEffect, power)
        if not hasEffect then
            return 1
        end
        return 1 + power / 1000
    end

    it('scarlet delirium multiplier', function()
        assert(scarletDeliriumMultiplier(false, 500) == 1)
        assert(scarletDeliriumMultiplier(true, 500) == 1.5)
        assert(scarletDeliriumMultiplier(true, 0) == 1)
    end)

    it('steam jacket element and mod gates', function()
        local mult, write = steamJacketMultiplier(xi.element.NONE, 25, xi.element.FIRE)
        assert(mult == 1 and write == false)
        mult, write = steamJacketMultiplier(xi.element.FIRE, 0, xi.element.FIRE)
        assert(mult == 1 and write == false)
    end)

    it('steam jacket track write and match reduction', function()
        local mult, write, tracked = steamJacketMultiplier(xi.element.FIRE, 25, xi.element.ICE)
        assert(mult == 1 and write == true and tracked == xi.element.FIRE)
        mult, write, tracked = steamJacketMultiplier(xi.element.THUNDER, 25, xi.element.THUNDER)
        assert(mult == 0.75 and write == true and tracked == xi.element.THUNDER)
        assert(xi.element.FIRE == 1 and xi.element.DARK == 8)
    end)
end)
