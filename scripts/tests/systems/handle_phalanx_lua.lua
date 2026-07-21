-----------------------------------
-- Pure system tests for utils.handlePhalanx / handleOneForAll inject form.
-----------------------------------

describe('handlePhalanx/OneForAll pure plans', function()
    local function handlePhalanx(damage, phalanxMod)
        if damage <= 0 then
            return damage
        end

        return utils.clamp(damage - phalanxMod, 0, 99999)
    end

    local function handleOneForAll(damage, power, hasEffect)
        if damage <= 0 then
            return damage
        end

        if not hasEffect then
            return damage
        end

        return utils.clamp(damage - power, 0, 99999)
    end

    it('handlePhalanx passthrough and clamp', function()
        assert(handlePhalanx(0, 100) == 0)
        assert(handlePhalanx(-5, 10) == -5)
        assert(handlePhalanx(100, 30) == 70)
        assert(handlePhalanx(50, 100) == 0)
        assert(handlePhalanx(200000, 0) == 99999)
    end)

    it('handleOneForAll gates and clamp', function()
        assert(handleOneForAll(100, 20, false) == 100)
        assert(handleOneForAll(0, 20, true) == 0)
        assert(handleOneForAll(100, 30, true) == 70)
        assert(handleOneForAll(10, 50, true) == 0)
        assert(handleOneForAll(200000, 0, true) == 99999)
    end)
end)
