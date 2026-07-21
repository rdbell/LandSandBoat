-----------------------------------
-- Pure system tests for calculateResistanceFactor inject (slice 6086).
-----------------------------------

describe('resistance factor pure injects', function()
    local function maxResistTier(isPC, elementalMeva)
        if not isPC then
            return 3
        end
        if elementalMeva < 0 then
            return 1
        elseif elementalMeva == 0 then
            return 2
        end
        return 3
    end

    local function countResistTiers(maxTiers, rolls)
        local tier = 0
        for i = 1, maxTiers do
            if not rolls[i] then
                break
            end
            tier = tier + 1
        end
        return tier
    end

    local function resistanceFactor(p)
        if p.hasMagicShield then
            return 0
        end
        if p.magicalElement == xi.element.NONE then
            return 1
        end
        local maxTiers = maxResistTier(p.isPC, p.elementalMeva or 0)
        local tier = countResistTiers(maxTiers, p.rolls or {})
        return 1 / (2 ^ tier)
    end

    it('magic shield and non-elemental early returns', function()
        assert(resistanceFactor({ hasMagicShield = true, magicalElement = xi.element.FIRE }) == 0)
        assert(resistanceFactor({
            hasMagicShield = false, magicalElement = xi.element.NONE, rolls = { true, true, true },
        }) == 1)
    end)

    it('tier loop composition', function()
        assert(resistanceFactor({
            magicalElement = xi.element.FIRE, rolls = { false, true, true },
        }) == 1)
        assert(resistanceFactor({
            magicalElement = xi.element.FIRE, rolls = { true, true, false },
        }) == 0.25)
        assert(resistanceFactor({
            magicalElement = xi.element.FIRE, isPC = true, elementalMeva = -10,
            rolls = { true, true, true },
        }) == 0.5)
    end)
end)
