-----------------------------------
-- Pure system tests for MTDR / staff / affinity / additional resist tier
-- inject halves (slice 6090).
-----------------------------------

describe('spell damage MTDR staff affinity pure injects', function()
    local function mtdr(isPC, targetAmount)
        if not isPC then
            return 1
        end
        if targetAmount == 1 then
            return 1
        end
        return utils.clamp(0.9 - 0.05 * targetAmount, 0.4, 1)
    end

    local function elementalStaff(spellElement, staffMod)
        if spellElement == xi.element.NONE then
            return 1
        end
        return 1 + staffMod * 5 / 100
    end

    local function elementalAffinity(spellElement, elementalMABMod)
        if spellElement == xi.element.NONE then
            return 1
        end
        return 1 + elementalMABMod / 100
    end

    local function additionalResistTier(hasSubtleSorcery, resistanceRank)
        if hasSubtleSorcery then
            return 1
        end
        if resistanceRank < 4 then
            return 1
        end
        return 0.5
    end

    it('MTDR PC multi-target clamp', function()
        assert(mtdr(false, 5) == 1)
        assert(mtdr(true, 1) == 1)
        assert(mtdr(true, 2) == 0.8)
        assert(mtdr(true, 12) == 0.4)
    end)

    it('elemental staff and affinity', function()
        assert(elementalStaff(xi.element.NONE, 10) == 1)
        assert(elementalStaff(xi.element.FIRE, 2) == 1.10)
        assert(elementalAffinity(xi.element.NONE, 50) == 1)
        assert(elementalAffinity(xi.element.FIRE, 25) == 1.25)
    end)

    it('additional resist tier', function()
        assert(additionalResistTier(true, 11) == 1)
        assert(additionalResistTier(false, 3) == 1)
        assert(additionalResistTier(false, 4) == 0.5)
        assert(additionalResistTier(false, 11) == 0.5)
    end)
end)
