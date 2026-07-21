-----------------------------------
-- Pure system tests for calculateTargetMagicEvasion inject form (slice 6083).
-----------------------------------

describe('target magic evasion pure injects', function()
    local resistRankMultiplier =
    {
        [-3] = 0.95,
        [-2] = 0.96019,
        [-1] = 0.98,
        [ 0] = 1,
        [ 1] = 1.023,
        [ 2] = 1.049,
        [ 3] = 1.0905,
        [ 4] = 1.126,
        [ 5] = 1.2075,
        [ 6] = 1.3475,
        [ 7] = 1.70065,
        [ 8] = 2.141,
        [ 9] = 2.2,
        [10] = 2.275,
        [11] = 2.35,
    }

    local function levelCorrectionMeva(targetIsPC, zoneLevelCorrected, targetLvl, actorLvl)
        if targetIsPC or not zoneLevelCorrected then
            return 0
        end
        local dlvl = targetLvl - actorLvl
        if dlvl < 0 then
            dlvl = 0
        end
        if dlvl > 100 then
            dlvl = 100
        end
        return dlvl * 4
    end

    local function targetMagicEvasion(p)
        local magicEva = p.baseMeva
        if p.magicalElement ~= xi.element.NONE then
            magicEva = magicEva + (p.elementalMevaMod or 0)
        end
        if p.effectId and p.effectId > 0 then
            magicEva = magicEva + (p.effectMevaMod or 0) + (p.statusMevaMod or 0)
        end
        magicEva = magicEva + levelCorrectionMeva(
            p.targetIsPC, p.zoneLevelCorrected, p.targetLvl or 0, p.actorLvl or 0)
        local rank = p.resistanceRank or 0
        local mult = resistRankMultiplier[rank] or resistRankMultiplier[0]
        return math.floor(magicEva * mult)
    end

    it('base and elemental MEVA', function()
        assert(targetMagicEvasion({ baseMeva = 200, magicalElement = xi.element.NONE, resistanceRank = 0 }) == 200)
        assert(targetMagicEvasion({
            baseMeva = 200, magicalElement = xi.element.FIRE, elementalMevaMod = 50, resistanceRank = 0,
        }) == 250)
        assert(targetMagicEvasion({
            baseMeva = 200, magicalElement = xi.element.NONE, elementalMevaMod = 50, resistanceRank = 0,
        }) == 200)
    end)

    it('status and level correction', function()
        assert(targetMagicEvasion({
            baseMeva = 100, magicalElement = xi.element.NONE, effectId = 0,
            effectMevaMod = 20, statusMevaMod = 10, resistanceRank = 0,
        }) == 100)
        assert(targetMagicEvasion({
            baseMeva = 100, magicalElement = xi.element.NONE, effectId = 2,
            effectMevaMod = 20, statusMevaMod = 10, resistanceRank = 0,
        }) == 130)
        assert(targetMagicEvasion({
            baseMeva = 100, magicalElement = xi.element.NONE,
            targetIsPC = false, zoneLevelCorrected = true,
            targetLvl = 80, actorLvl = 70, resistanceRank = 0,
        }) == 140)
        assert(targetMagicEvasion({
            baseMeva = 100, magicalElement = xi.element.NONE,
            targetIsPC = true, zoneLevelCorrected = true,
            targetLvl = 80, actorLvl = 70, resistanceRank = 0,
        }) == 100)
    end)

    it('resistance rank scale and full composition', function()
        assert(targetMagicEvasion({
            baseMeva = 100, magicalElement = xi.element.NONE, resistanceRank = 1,
        }) == 102)
        assert(targetMagicEvasion({
            baseMeva = 200, magicalElement = xi.element.WIND, elementalMevaMod = 30,
            effectId = 5, effectMevaMod = 15, statusMevaMod = 10,
            targetIsPC = false, zoneLevelCorrected = true,
            targetLvl = 75, actorLvl = 70, resistanceRank = 0,
        }) == 275)
    end)
end)
