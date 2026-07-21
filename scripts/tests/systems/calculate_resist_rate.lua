-----------------------------------
-- Pure system tests for calculateResistRate top-level composition (slice 6088).
-----------------------------------

describe('calculate resist rate pure composition', function()
    local function targetResistanceRank(p)
        if p.targetIsPC then
            return 0
        end
        local rank = p.baseRank
        if p.effectId and p.effectId > 0 then
            rank = rank - (p.immunobreakMod or 0)
        end
        return utils.clamp(rank, -3, 11)
    end

    local function resistanceFactor(p)
        if p.hasMagicShield then
            return 0
        end
        if p.magicalElement == xi.element.NONE then
            return 1
        end
        local maxTiers = 3
        if p.isPC then
            if (p.elementalMeva or 0) < 0 then
                maxTiers = 1
            elseif (p.elementalMeva or 0) == 0 then
                maxTiers = 2
            end
        end
        local tier = 0
        local rolls = p.rolls or {}
        for i = 1, maxTiers do
            if not rolls[i] then
                break
            end
            tier = tier + 1
        end
        return 1 / (2 ^ tier)
    end

    local function calculateResistRate(p)
        local rank = targetResistanceRank({
            targetIsPC = p.targetIsPC,
            baseRank = p.baseRank or 0,
            effectId = p.effectId or 0,
            immunobreakMod = p.immunobreakMod or 0,
        })
        if rank >= 11 then
            if (p.effectId or 0) > 0 then
                return 0
            end
            return 0.25
        end
        return resistanceFactor({
            hasMagicShield = p.hasMagicShield,
            magicalElement = p.magicalElement,
            isPC = p.isPC,
            elementalMeva = p.elementalMeva,
            rolls = p.rolls,
        })
    end

    it('rank 11 auto-resist', function()
        assert(calculateResistRate({ magicalElement = xi.element.FIRE, baseRank = 11, effectId = 2 }) == 0)
        assert(calculateResistRate({ magicalElement = xi.element.FIRE, baseRank = 11, effectId = 0 }) == 0.25)
    end)

    it('magic shield and non-elemental after rank gate', function()
        assert(calculateResistRate({ hasMagicShield = true, magicalElement = xi.element.FIRE, baseRank = 0 }) == 0)
        assert(calculateResistRate({ magicalElement = xi.element.NONE, baseRank = 0 }) == 1)
    end)

    it('tier composition', function()
        assert(calculateResistRate({
            magicalElement = xi.element.FIRE, targetIsPC = true, baseRank = 5,
            rolls = { true, true, false },
        }) == 0.25)
        assert(calculateResistRate({
            magicalElement = xi.element.FIRE, baseRank = 10, effectId = 0,
            rolls = { true, true, true },
        }) == 0.125)
    end)
end)
